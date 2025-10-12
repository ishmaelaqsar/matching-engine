#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/log/trivial.hpp>
#include <matchingengine/protocol/admin/login.h>
#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/tcp/connection.h>
#include <optional>

namespace tcp
{
    Connection::Connection(
        const core::ConnectionId id, boost::asio::io_context& io_context,
        boost::asio::ip::tcp::socket socket,
        std::function<bool(const std::string&)> username_check,
        std::function<void(core::Payload)> request_enqueue,
        std::function<void(Command)> command_enqueue
    )
        : f_id(id), f_username_check(std::move(username_check)),
          f_request_enqueue(std::move(request_enqueue)),
          f_command_enqueue(std::move(command_enqueue)),
          f_socket(std::move(socket)), f_io_context(io_context)
    {
    }

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
            f_socket, boost::asio::buffer(f_header_buffer),
            [self = shared_from_this(
             )](const boost::system::error_code& ec, size_t) {
                if (ec) {
                    self->error(ec);
                    self->f_command_enqueue(StopCommand{self->f_id});
                    return;
                }

                const auto [type, length] = core::protocol::Header::deserialize(
                    self->f_header_buffer.data()
                );

                if (type != core::protocol::admin::LoginRequest::type()) {
                    self->error("Expected LoginRequest, got type=" + type);
                    self->f_command_enqueue(StopCommand{self->f_id});
                    return;
                }

                self->f_payload_buffer.resize(length);
                boost::asio::async_read(
                    self->f_socket,
                    boost::asio::buffer(self->f_payload_buffer.data(), length),
                    [self](const boost::system::error_code& ec, size_t) {
                        if (ec) {
                            self->error(ec);
                            self->f_command_enqueue(StopCommand{self->f_id});
                            return;
                        }

                        // Deserialize LoginRequest
                        core::protocol::admin::LoginRequest login_req;
                        login_req.deserialize(self->f_payload_buffer.data());
                        self->f_username = login_req.username();

                        auto success = true;
                        std::string error_msg{};

                        if (self->f_username.empty()) {
                            success = false;
                            error_msg = "Missing username";
                        } else if (!self->f_username_check(self->f_username)) {
                            success = false;
                            error_msg = "Username already logged in";
                        }

                        const core::protocol::admin::LoginResponse login_resp(
                            self->f_username, success
                        );

                        const auto resp_header = core::protocol::Header{
                            core::protocol::admin::LoginResponse::type(),
                            login_resp.size()
                        };

                        core::Data resp_data;
                        login_resp.serialize(resp_data.data());
                        self->write(resp_header, resp_data);

                        if (success) {
                            self->f_command_enqueue(
                                InitCommand{self->f_id, self->f_username}
                            );
                            self->read_header();
                        } else {
                            self->error(std::move(error_msg));
                            self->f_command_enqueue(StopCommand{self->f_id});
                        }
                    }
                );
            }
        );
    }

    void Connection::stop()
    {
        f_stopped.store(true);
    }

    void Connection::write(
        const core::protocol::Header& header, const core::Data& data
    )
    {
        const auto total_size = core::protocol::Header::Size + header.length;
        const auto buffer =
            std::make_shared<std::vector<unsigned char>>(total_size);

        core::protocol::Header::serialize(header, buffer->data());
        std::memcpy(
            buffer->data() + core::protocol::Header::Size, data.data(),
            header.length
        );

        boost::asio::async_write(
            f_socket, boost::asio::buffer(*buffer, total_size),
            [self = shared_from_this(),
             buffer](const boost::system::error_code& ec, size_t) -> void {
                handle_write(ec, buffer);
            }
        );
    }

    void Connection::handle_write(
        const boost::system::error_code& error_code,
        const std::shared_ptr<std::vector<unsigned char>>& data
    )
    {
        if (error_code) {
            BOOST_LOG_TRIVIAL(error) << "Write error: " << error_code.message();
        } else {
            BOOST_LOG_TRIVIAL(debug)
                << "Write success, " << data->size() << " bytes.";
        }
    }

    void Connection::read_header()
    {
        boost::asio::async_read(
            f_socket, boost::asio::buffer(f_header_buffer),
            [self = shared_from_this(
             )](const boost::system::error_code& ec, const size_t) -> void {
                if (ec) {
                    BOOST_LOG_TRIVIAL(error) << "Read error: " << ec.message();
                    self->f_command_enqueue(
                        ErrorCommand{
                            self->f_id,
                            core::protocol::info::Error::server_error()
                        }
                    );
                    self->f_command_enqueue(StopCommand{self->f_id});
                    return;
                }
                const auto header = core::protocol::Header::deserialize(
                    self->f_header_buffer.data()
                );
                self->read_payload(header);
            }
        );
    }

    void Connection::read_payload(core::protocol::Header header)
    {
        f_payload_buffer.resize(header.length);

        boost::asio::async_read(
            f_socket,
            boost::asio::buffer(f_payload_buffer.data(), header.length),
            [self = shared_from_this(), header](
                const boost::system::error_code& ec, const size_t
            ) -> void {
                if (ec) {
                    BOOST_LOG_TRIVIAL(error) << "Read error: " << ec.message();
                    self->f_command_enqueue(
                        ErrorCommand{
                            self->f_id,
                            core::protocol::info::Error::server_error()
                        }
                    );
                    self->f_command_enqueue(StopCommand{self->f_id});
                    return;
                }
                auto data = core::Data{};
                if (core::MaxPayloadSize < header.length) {
                    BOOST_LOG_TRIVIAL(warning)
                        << "Request size is too big, dropping "
                           "message.";
                } else {
                    std::memcpy(
                        data.data(), self->f_payload_buffer.data(),
                        header.length
                    );
                    const auto payload =
                        core::Payload{self->f_id, header, data};
                    self->f_request_enqueue(payload);
                }

                if (!self->f_stopped.load()) {
                    // read the next message
                    self->read_header();
                }
            }
        );
    }

    void Connection::error(const boost::system::error_code& ec) const
    {
        BOOST_LOG_TRIVIAL(error) << "Read error: " << ec.message();
        f_command_enqueue(
            ErrorCommand{f_id, core::protocol::info::Error::server_error()}
        );
    }

    void Connection::error(std::string&& message) const
    {
        BOOST_LOG_TRIVIAL(error) << "Client error: " << message;
        f_command_enqueue(
            ErrorCommand{
                f_id,
                core::protocol::info::Error::client_error(std::move(message))
            }
        );
    }
} // namespace tcp
