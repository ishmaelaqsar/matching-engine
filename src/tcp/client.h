#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>
#include <iostream>

#include "../common/protocol/trading/add_order.h"
#include "header.h"

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
                                if (ec) std::cerr << ec.message() << "\n";
                                f_socket.close(ec); // NOLINT(*-unused-return-value)
                                if (ec) std::cerr << ec.message() << "\n";
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

                common::protocol::trading::AddOrderResponse
                add_order(const common::protocol::trading::AddOrderRequest &request)
                {
                        if (!f_connected) throw std::runtime_error("Not connected");

                        const size_t buffer_size = MessageHeader::Size + request.size();
                        const auto buffer = new char[buffer_size];

                        MessageHeader::serialize({common::MessageType::AddOrderRequest, request.size()}, buffer);
                        common::protocol::trading::AddOrderRequest::serialize(request, buffer + MessageHeader::Size);

                        f_socket.send(boost::asio::buffer(buffer, buffer_size));
                        delete[] buffer;

                        return add_order_response();
                }

        private:
                common::protocol::trading::AddOrderResponse add_order_response()
                {
                        std::array<char, MessageHeader::Size> header_buffer{};
                        f_socket.receive(boost::asio::buffer(header_buffer));
                        auto [type, length] = MessageHeader::deserialize(header_buffer.data());

                        if (type != common::MessageType::AddOrderResponse) {
                                throw std::runtime_error("Unexpected message type");
                        }

                        const auto payload_buffer = new char[length];

                        f_socket.receive(boost::asio::buffer(payload_buffer, length));
                        const auto response = common::protocol::trading::AddOrderResponse::deserialize(payload_buffer);

                        delete[] payload_buffer;

                        return response;
                }

                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::socket f_socket;
                bool f_connected = false;
        };
} // namespace tcp

#endif // CLIENT_H
