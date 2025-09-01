#include <boost/log/trivial.hpp>

#include "tcp/server.h"

namespace tcp
{
        static constexpr unsigned short DefaultPort = 8080;
        static constexpr size_t BatchSize = 1024;

        Server::Server(const std::shared_ptr<core::RingBuffer<core::Payload>> &inbound_buffer,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &outbound_buffer,
                       boost::asio::io_context &io_context) :
            Server(DefaultPort, inbound_buffer, outbound_buffer, io_context)
        {}

        Server::Server(const unsigned short &port,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &inbound_buffer,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &outbound_buffer,
                       boost::asio::io_context &io_context) :
            f_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
            f_io_context(io_context),
            f_inbound_buffer(inbound_buffer),
            f_outbound_buffer(outbound_buffer)
        {}

        void Server::start()
        {
                BOOST_LOG_TRIVIAL(info) << "Starting server on: " << f_acceptor.local_endpoint();
                f_running = true;
                accept_connections();
                f_polling_thread = std::thread([this]() -> void {
                        auto empty_count = 0;
                        while (f_running.load(std::memory_order_relaxed)) {
                                if (!f_outbound_buffer->empty()) {
                                        poll_responses();
                                        empty_count = 0;
                                } else {
                                        empty_count++;
                                        if (empty_count > 1000) {
                                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                                        } else {
                                                std::this_thread::yield();
                                        }
                                }
                        }
                });
        }

        void Server::stop()
        {
                BOOST_LOG_TRIVIAL(info) << "Stopping server";
                f_running = false;
                f_acceptor.close();

                if (f_polling_thread.joinable()) {
                        f_polling_thread.join();
                }

                f_connections.clear();
        }

        bool Server::is_running() const
        {
                return f_running;
        }

        void Server::queue_request(const core::Payload &request) const
        {
                if (!f_inbound_buffer->push(request)) {
                        BOOST_LOG_TRIVIAL(error) << "Inbound queue full, dropping message.";
                }
        }

        void Server::queue_response(const core::Payload &response) const
        {
                if (!f_outbound_buffer->push(response)) {
                        BOOST_LOG_TRIVIAL(error) << "Outbound queue full, dropping message.";
                }
        }

        void Server::accept_connections()
        {
                f_acceptor.async_accept(
                        [this](const boost::system::error_code &ec, boost::asio::ip::tcp::socket socket) -> void {
                                if (!ec) {
                                        BOOST_LOG_TRIVIAL(info) << "New connection accepted";

                                        const auto connection_id = ++f_counter;
                                        BOOST_LOG_TRIVIAL(info) << "Creating new connection, ID=" << connection_id;
                                        const auto connection = std::make_shared<Connection>(
                                                connection_id, f_io_context, std::move(socket),
                                                [self = shared_from_this()](const core::Payload &payload) -> void {
                                                        if (!self->f_inbound_buffer->push(payload)) {
                                                                BOOST_LOG_TRIVIAL(error)
                                                                        << "Inbound queue full, dropping message.";
                                                        }
                                                },
                                                [self = shared_from_this()](const core::ConnectionId &cid) -> void {
                                                        self->f_connections.erase(cid);
                                                        BOOST_LOG_TRIVIAL(info) << "Removed connection, ID=" << cid;
                                                });

                                        f_connections[connection_id] = connection;

                                        BOOST_LOG_TRIVIAL(info) << "Starting new connection, ID=" << connection->id();
                                        connection->start();
                                } else {
                                        BOOST_LOG_TRIVIAL(error) << ec.message();
                                }
                                accept_connections(); // Accept next connection
                        });
        }

        void Server::poll_responses() const
        {
                int work_left = BatchSize;
                core::Payload payload{};

                while (work_left-- && f_outbound_buffer->pop(payload)) {
                        f_io_context.post([self = shared_from_this(), payload]() -> void {
                                if (const auto entry = self->f_connections.find(payload.connectionId);
                                    entry != self->f_connections.end()) {
                                        const auto connection = entry->second;
                                        connection->write(payload.header, payload.data);
                                } else {
                                        BOOST_LOG_TRIVIAL(warning)
                                                << "Connection ID=" << payload.connectionId << " not found";
                                }
                        });
                }
        }
} // namespace tcp
