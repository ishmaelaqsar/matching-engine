#include <boost/log/trivial.hpp>
#include <matchingengine/tcp/server.h>

namespace tcp
{
    static constexpr unsigned short DefaultPort = 8080;
    static constexpr size_t BatchSize = 1024;

    Server::Server(
        core::RingBuffer<core::Payload>& inbound_buffer,
        core::RingBuffer<core::Payload>& outbound_buffer,
        boost::asio::io_context& io_context
    )
        : Server(DefaultPort, inbound_buffer, outbound_buffer, io_context)
    {
    }

    Server::Server(
        const unsigned short& port,
        core::RingBuffer<core::Payload>& inbound_buffer,
        core::RingBuffer<core::Payload>& outbound_buffer,
        boost::asio::io_context& io_context
    )
        : f_acceptor(
              io_context,
              boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
          ),
          f_io_context(io_context), f_timer(io_context),
          f_inbound_buffer(inbound_buffer), f_outbound_buffer(outbound_buffer)
    {
    }

    void Server::start()
    {
        BOOST_LOG_TRIVIAL(info)
            << "Starting server on: " << f_acceptor.local_endpoint();
        f_running = true;
        accept_connections();
        schedule_poll();
    }

    void Server::stop()
    {
        BOOST_LOG_TRIVIAL(info) << "Stopping server";
        f_running = false;
        f_acceptor.close();
        f_timer.cancel();

        f_active_connections.clear();
        f_connections_by_id.clear();
    }

    bool Server::is_running() const
    {
        return f_running;
    }

    void Server::accept_connections()
    {
        f_acceptor.async_accept(
            [this](
                const boost::system::error_code& ec,
                boost::asio::ip::tcp::socket socket
            ) -> void {
                if (!ec) {
                    BOOST_LOG_TRIVIAL(info) << "New connection accepted";

                    const auto connection_id = ++f_counter;
                    BOOST_LOG_TRIVIAL(info)
                        << "Creating new connection, ID=" << connection_id;

                    const auto self = shared_from_this();
                    auto username_check =
                        [self](const std::string& username) -> bool {
                        if (self->f_active_connections.right.count(username) >
                            0) {
                            return false;
                        }
                        return true;
                    };
                    auto request_enqueue =
                        [self](const core::Payload& payload) -> void {
                        if (!self->f_inbound_buffer.push(payload)) {
                            BOOST_LOG_TRIVIAL(error) << "Inbound queue full, "
                                                        "dropping message.";
                        }
                    };
                    auto command_enqueue = [self](Command&& command) -> void {
                        if (self->f_command_queue.full()) {
                            BOOST_LOG_TRIVIAL(warning) << "Command queue full, "
                                                          "dropping command";
                        } else {
                            self->f_command_queue.push_back(std::move(command));
                        }
                    };
                    const auto connection = std::make_shared<Connection>(
                        connection_id, f_io_context, std::move(socket),
                        username_check, request_enqueue, command_enqueue
                    );

                    f_connections_by_id[connection_id] = connection;

                    BOOST_LOG_TRIVIAL(info)
                        << "Starting new connection, ID=" << connection->id();
                    connection->start();
                } else {
                    BOOST_LOG_TRIVIAL(error) << ec.message();
                }
                accept_connections(); // Accept next connection
            }
        );
    }

    void Server::schedule_poll()
    {
        f_timer.expires_after(std::chrono::seconds(1));
        f_timer.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                poll_responses();
                if (is_running()) {
                    schedule_poll();
                }
            }
        });
    }

    void Server::poll_responses()
    {
        auto work_left = BatchSize;

        while (work_left-- && !f_command_queue.empty()) {
            auto cmd = std::move(f_command_queue.front());
            f_command_queue.pop_front();

            std::visit(
                [this](auto&& c) -> void {
                    using T = std::decay_t<decltype(c)>;
                    if constexpr (std::is_same_v<T, InitCommand>) {
                        f_active_connections.insert({c.id, c.username});
                        BOOST_LOG_TRIVIAL(info)
                            << "Connection ID=" << c.id
                            << " connected as user: " << c.username;
                    } else if constexpr (std::is_same_v<T, InfoCommand>) {
                        const auto header = core::protocol::Header{
                            core::protocol::MessageType::Info, c.message.size()
                        };
                        auto data = core::Data{};
                        c.message.serialize(data.data());
                        send_message(c.id, header, data);
                    } else if constexpr (std::is_same_v<T, ErrorCommand>) {
                        const auto header = core::protocol::Header{
                            core::protocol::MessageType::Error, c.message.size()
                        };
                        auto data = core::Data{};
                        c.message.serialize(data.data());
                        send_message(c.id, header, data);
                    } else if constexpr (std::is_same_v<T, StopCommand>) {
                        f_active_connections.left.erase(c.id);
                        f_connections_by_id.erase(c.id);
                        BOOST_LOG_TRIVIAL(info)
                            << "Removed connection ID=" << c.id;
                    }
                },
                cmd
            );
        }

        core::Payload payload{};
        while (work_left-- && f_outbound_buffer.pop(payload)) {
            send_message(payload.connection_id, payload.header, payload.data);
        }
    }

    void Server::send_message(
        const core::ConnectionId& id, const core::protocol::Header& header,
        const core::Data& data
    )
    {
        if (const auto entry = f_connections_by_id.find(id);
            entry != f_connections_by_id.end()) {
            const auto connection = entry->second;
            connection->write(header, data);
        } else {
            BOOST_LOG_TRIVIAL(warning)
                << "Connection ID=" << id << " not found";
        }
    }
} // namespace tcp
