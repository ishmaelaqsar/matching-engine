#include <matchingengine/tcp/connection.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/log/trivial.hpp>

#include <matchingengine/protocol/serialize_helper.h>

namespace tcp
{
        Connection::Connection(const core::ConnectionId id, boost::asio::io_context &io_context,
                               boost::asio::ip::tcp::socket socket,
                               std::function<bool(const std::string &)> username_check,
                               std::function<void(core::Payload)> request_enqueue,
                               std::function<void(core::ConnectionId, InfoMessages)> on_info,
                               std::function<void(core::ConnectionId)> on_fail) :
            f_id(id),
            f_username_check(std::move(username_check)),
            f_request_enqueue(std::move(request_enqueue)),
            f_on_info(std::move(on_info)),
            f_on_fail(std::move(on_fail)),
            f_socket(std::move(socket)),
            f_io_context(io_context)
        {}

        core::ConnectionId Connection::id() const
        {
                return f_id;
        }

        std::string_view Connection::username() const
        {
                return f_username;
        }

        void Connection::start()
        {
                boost::asio::async_read(
                        f_socket, boost::asio::buffer(f_username_buffer),
                        [self = shared_from_this()](const boost::system::error_code &ec, const size_t) -> void {
                                if (ec) {
                                        BOOST_LOG_TRIVIAL(error) << "Read username error: " << ec.message();
                                        self->f_on_info(self->f_id, core::protocol::info::Error::client_error());
                                        self->f_on_fail(self->f_id);
                                        return;
                                }

                                size_t offset = 0;
                                self->f_username =
                                        core::protocol::deserialize_string(self->f_username_buffer.data(), &offset);

                                if (self->f_username.empty()) {
                                        BOOST_LOG_TRIVIAL(error) << "Invalid username received";
                                        self->f_on_info(self->f_id,
                                                        core::protocol::info::Error::client_error("Invalid Username"));
                                        self->f_on_fail(self->f_id);
                                        return;
                                }

                                if (!self->f_username_check(self->f_username)) {
                                        BOOST_LOG_TRIVIAL(error) << "Username already logged in: " << self->f_username;
                                        self->f_on_info(self->f_id, core::protocol::info::Error::client_error(
                                                                            "Username already logged in"));
                                        self->f_on_fail(self->f_id);
                                        return;
                                }

                                BOOST_LOG_TRIVIAL(info)
                                        << "Connection ID=" << self->f_id << " connected as user: " << self->f_username;

                                self->f_on_info(self->f_id,
                                                core::protocol::info::Info("Logged in as " + self->f_username));

                                self->read_header();
                        });
        }

        void Connection::stop()
        {
                f_stopped.store(true);
        }

        void Connection::write(const core::protocol::Header &header, const core::Data &data)
        {
                const auto total_size = core::protocol::Header::Size + header.length;
                const auto buffer = std::make_shared<std::vector<unsigned char>>(total_size);

                core::protocol::Header::serialize(header, buffer->data());
                std::memcpy(buffer->data() + core::protocol::Header::Size, data.data(), header.length);

                boost::asio::async_write(
                        f_socket, boost::asio::buffer(*buffer, total_size),
                        [self = shared_from_this(), buffer](const boost::system::error_code ec, size_t) -> void {
                                handle_write(ec, buffer);
                        });
        }

        void Connection::handle_write(const boost::system::error_code &error_code,
                                      const std::shared_ptr<std::vector<unsigned char>> &data)
        {
                if (error_code) {
                        BOOST_LOG_TRIVIAL(error) << "Write error: " << error_code.message();
                } else {
                        BOOST_LOG_TRIVIAL(debug) << "Write success, " << data->size() << " bytes.";
                }
        }

        void Connection::read_header()
        {
                boost::asio::async_read(
                        f_socket, boost::asio::buffer(f_header_buffer),
                        [self = shared_from_this()](const boost::system::error_code &ec, const size_t) -> void {
                                if (ec) {
                                        BOOST_LOG_TRIVIAL(error) << "Read error: " << ec.message();
                                        self->f_on_fail(self->f_id);
                                        return;
                                }
                                const auto header = core::protocol::Header::deserialize(self->f_header_buffer.data());
                                self->read_payload(header);
                        });
        }

        void Connection::read_payload(core::protocol::Header header)
        {
                f_payload_buffer.resize(header.length);

                boost::asio::async_read(
                        f_socket, boost::asio::buffer(f_payload_buffer.data(), header.length),
                        [self = shared_from_this(), header](const boost::system::error_code &ec, const size_t) -> void {
                                if (ec) {
                                        BOOST_LOG_TRIVIAL(error) << "Read error: " << ec.message();
                                        self->f_on_fail(self->f_id);
                                        return;
                                }
                                auto data = core::Data{};
                                if (core::MaxPayloadSize < header.length) {
                                        BOOST_LOG_TRIVIAL(warning) << "Request size is too big, dropping message.";
                                } else {
                                        std::memcpy(data.data(), self->f_payload_buffer.data(), header.length);
                                        const auto payload = core::Payload{self->f_id, header, data};
                                        self->f_request_enqueue(payload);
                                }

                                if (!self->f_stopped.load()) {
                                        // read the next message
                                        self->read_header();
                                }
                        });
        }
} // namespace tcp
