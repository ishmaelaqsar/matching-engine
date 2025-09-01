#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <unordered_map>

#include "../common/protocol/header.h"
#include "../common/protocol/trading/add_order.h"
#include "common/protocol/view/get_book.h"

namespace tcp
{
        // NOT THREAD SAFE
        class Client
        {
        public:
                Client(boost::asio::io_context &io_context, const std::string &host, const unsigned short port) :
                    f_endpoint({boost::asio::ip::make_address(host), port}),
                    f_io_context(io_context),
                    f_socket(boost::asio::ip::tcp::socket(io_context))
                {}

                ~Client()
                {
                        disconnect();
                }

                void connect()
                {
                        try {
                                f_socket.connect(f_endpoint);
                                f_connected = true;
                        } catch (const boost::system::system_error &e) {
                                f_connected = false;
                                throw std::runtime_error("Connection failed: " + std::string(e.what()));
                        }
                }

                void disconnect()
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

                [[nodiscard]] bool is_connected() const
                {
                        return f_connected;
                }

                explicit operator bool() const
                {
                        return f_connected;
                }

                std::shared_ptr<common::protocol::Message>
                request(const std::unique_ptr<common::protocol::Message> &request)
                {
                        if (!f_connected) throw std::runtime_error("Not connected");

                        const size_t buffer_size = common::protocol::Header::Size + request->size();
                        const auto buffer = new unsigned char[buffer_size];

                        common::protocol::Header::serialize({request->type(), request->size()}, buffer);
                        request->serialize(buffer + common::protocol::Header::Size);

                        f_socket.send(boost::asio::buffer(buffer, buffer_size));
                        delete[] buffer;

                        return response(request->type());
                }

        private:
                std::shared_ptr<common::protocol::Message> response(const common::protocol::MessageType requestType)
                {
                        std::array<unsigned char, common::protocol::Header::Size> header_buffer{};
                        f_socket.receive(boost::asio::buffer(header_buffer));
                        auto [type, length] = common::protocol::Header::deserialize(header_buffer.data());

                        const auto payload_buffer = new unsigned char[length];
                        auto response = f_request_to_response_type[requestType];

                        f_socket.receive(boost::asio::buffer(payload_buffer, length));
                        response->deserialize(payload_buffer);

                        delete[] payload_buffer;

                        return response;
                }

                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::socket f_socket;
                bool f_connected = false;
                std::unordered_map<common::protocol::MessageType, std::shared_ptr<common::protocol::Message>>
                        f_request_to_response_type = {{common::protocol::MessageType::AddOrderRequest,
                                                       std::make_shared<common::protocol::trading::AddOrderResponse>()},
                                                      {common::protocol::MessageType::GetBookRequest,
                                                       std::make_shared<common::protocol::view::GetBookResponse>()}};
        };
} // namespace tcp

#endif // CLIENT_H
