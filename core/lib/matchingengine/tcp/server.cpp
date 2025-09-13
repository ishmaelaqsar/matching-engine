#include <matchingengine/tcp/server.h>

#include <boost/log/trivial.hpp>

namespace tcp
{
        static constexpr unsigned short DefaultPort = 8080;
        static constexpr size_t BatchSize = 1024;

        Server::Server(core::RingBuffer<core::Payload> &inbound_buffer,
                       core::RingBuffer<core::Payload> &outbound_buffer, boost::asio::io_context &io_context) :
            Server(DefaultPort, inbound_buffer, outbound_buffer, io_context)
        {}

        Server::Server(const unsigned short &port, core::RingBuffer<core::Payload> &inbound_buffer,
                       core::RingBuffer<core::Payload> &outbound_buffer, boost::asio::io_context &io_context) :
            f_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
            f_io_context(io_context),
            f_timer(io_context),
            f_inbound_buffer(inbound_buffer),
            f_outbound_buffer(outbound_buffer)
        {}

        void Server::start()
        {
                BOOST_LOG_TRIVIAL(info) << "Starting server on: " << f_acceptor.local_endpoint();
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
                f_acceptor.async_accept([this](const boost::system::error_code &ec,
                                               boost::asio::ip::tcp::socket socket) -> void {
                        if (!ec) {
                                BOOST_LOG_TRIVIAL(info) << "New connection accepted";

                                const auto connection_id = ++f_counter;
                                BOOST_LOG_TRIVIAL(info) << "Creating new connection, ID=" << connection_id;
                                const auto self = shared_from_this();
                                const auto connection = std::make_shared<Connection>(
                                        connection_id, f_io_context, std::move(socket),
                                        [self, connection_id](const std::string &username) -> bool {
                                                if (self->f_active_connections.right.count(username) > 0) {
                                                        return false;
                                                }
                                                self->f_active_connections.insert({connection_id, username});
                                                return true;
                                        },
                                        [self](const core::Payload &payload) -> void {
                                                if (!self->f_inbound_buffer.push(payload)) {
                                                        BOOST_LOG_TRIVIAL(error)
                                                                << "Inbound queue full, dropping message.";
                                                }
                                        },
                                        [self](const core::ConnectionId &cid, const InfoMessages &message) -> void {
                                                InfoCommand command{std::make_pair(cid, message)};
                                                // if (const Messages msg = command; !self->f_message_queue.push(msg)) {
                                                //         BOOST_LOG_TRIVIAL(error)
                                                //                 << "Connection queue full, dropping message.";
                                                // }
                                        },
                                        [self](const core::ConnectionId &cid) -> void {
                                                StopCommand command{cid};
                                                // if (const Messages msg = command; !self->f_message_queue.push(msg)) {
                                                //         BOOST_LOG_TRIVIAL(error)
                                                //                 << "Connection queue full, dropping message.";
                                                // }
                                        });

                                f_connections_by_id[connection_id] = connection;

                                BOOST_LOG_TRIVIAL(info) << "Starting new connection, ID=" << connection->id();
                                connection->start();
                        } else {
                                BOOST_LOG_TRIVIAL(error) << ec.message();
                        }
                        accept_connections(); // Accept next connection
                });
        }

        void Server::schedule_poll()
        {
                f_timer.expires_after(std::chrono::seconds(1));
                f_timer.async_wait([this](const boost::system::error_code &ec) {
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

                // Messages message_variant{};
                // while (work_left-- && f_message_queue.pop(message_variant)) {
                //         f_io_context.post([self = shared_from_this(), message_variant]() -> void {
                //                 std::visit(
                //                         [self](auto &&arg) {
                //                                 using T = std::decay_t<decltype(arg)>;
                //                                 if constexpr (std::is_same_v<T, InfoCommand>) {
                //                                         auto connection_id = arg.id_message.first;
                //                                         auto info_message_variant = arg.id_message.second;
                //                                         std::visit(
                //                                                 [self, connection_id](auto &&msg) {
                //                                                         const auto entry =
                //                                                                 self->f_connections_by_id.find(
                //                                                                         connection_id);
                //                                                         if (entry == self->f_connections_by_id.end())
                //                                                         {
                //                                                                 BOOST_LOG_TRIVIAL(warning)
                //                                                                         << "Connection ID="
                //                                                                         << connection_id
                //                                                                         << " not found for sending "
                //                                                                            "info message.";
                //                                                                 return;
                //                                                         }
                //
                //                                                         const auto connection = entry->second;
                //
                //                                                         core::Data data{};
                //                                                         auto header = core::protocol::Header{
                //                                                                 msg.type(), msg.size()};
                //
                //                                                         msg.serialize(data.data());
                //                                                         connection->write(std::move(header), data);
                //                                                 },
                //                                                 info_message_variant);
                //                                 } else if constexpr (std::is_same_v<T, StopCommand>) {
                //                                         auto connection_id = arg.connection_id;
                //                                         const auto entry =
                //                                                 self->f_connections_by_id.find(connection_id);
                //                                         if (entry == self->f_connections_by_id.end()) {
                //                                                 BOOST_LOG_TRIVIAL(warning)
                //                                                         << "Connection ID=" << connection_id
                //                                                         << " not found for sending "
                //                                                            "stop message.";
                //                                                 return;
                //                                         }
                //
                //                                         const auto connection = entry->second;
                //                                         connection->stop();
                //                                         self->f_connections_by_id.erase(connection_id);
                //                                         BOOST_LOG_TRIVIAL(info)
                //                                                 << "Removed Connection ID=" << connection_id;
                //                                 }
                //                         },
                //                         message_variant);
                //         });
                // }

                core::Payload payload{};
                while (work_left-- && f_outbound_buffer.pop(payload)) {
                        if (const auto entry = f_connections_by_id.find(payload.connection_id);
                            entry != f_connections_by_id.end()) {
                                const auto connection = entry->second;
                                connection->write(payload.header, payload.data);
                        } else {
                                BOOST_LOG_TRIVIAL(warning) << "Connection ID=" << payload.connection_id << " not found";
                        }
                }
        }
} // namespace tcp
