#include "tcp/client.h"
#include "core/protocol/header.h"

namespace tcp
{
        inline void Client::connect()
        {
                try {
                        f_socket.connect(f_endpoint);
                        f_connected = true;
                } catch (const boost::system::system_error &e) {
                        f_connected = false;
                        throw std::runtime_error("Connection failed: " + std::string(e.what()));
                }
        }

        inline void Client::disconnect()
        {
                if (f_connected) {
                        boost::system::error_code ec;
                        f_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                                          ec); // NOLINT(*-unused-return-value)
                        if (ec) BOOST_LOG_TRIVIAL(error) << ec.message();
                        f_socket.close(ec); // NOLINT(*-unused-return-value)
                        if (ec) BOOST_LOG_TRIVIAL(error) << ec.message();
                        f_connected = false;
                }
        }

        inline bool Client::is_connected() const
        {
                return f_connected;
        }

        inline Client::operator bool() const
        {
                return f_connected;
        }

        inline std::shared_ptr<core::protocol::Message>
        Client::request(const std::unique_ptr<core::protocol::Message> &request)
        {
                if (!f_connected) throw std::runtime_error("Not connected");

                const size_t buffer_size = core::protocol::Header::Size + request->size();
                const auto buffer = new unsigned char[buffer_size];

                core::protocol::Header::serialize({request->type(), request->size()}, buffer);
                request->serialize(buffer + core::protocol::Header::Size);

                f_socket.send(boost::asio::buffer(buffer, buffer_size));
                delete[] buffer;

                return response(request->type());
        }

        inline std::shared_ptr<core::protocol::Message> Client::response(const core::protocol::MessageType requestType)
        {
                std::array<unsigned char, core::protocol::Header::Size> header_buffer{};
                f_socket.receive(boost::asio::buffer(header_buffer));
                auto [type, length] = core::protocol::Header::deserialize(header_buffer.data());

                const auto payload_buffer = new unsigned char[length];
                auto response = core::protocol::create(requestType);

                f_socket.receive(boost::asio::buffer(payload_buffer, length));
                response->deserialize(payload_buffer);

                delete[] payload_buffer;

                return response;
        }
} // namespace tcp
