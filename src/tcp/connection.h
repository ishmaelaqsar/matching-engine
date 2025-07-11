#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <chrono>
#include <memory>

#include "protocol/header.h"
#include "protocol/add_order.h"

namespace orderbook
{
        class TCPConnection : public std::enable_shared_from_this<TCPConnection>
        {
        public:
                explicit TCPConnection(boost::asio::io_context &io_context) :
                        f_socket(io_context) {}

                TCPConnection(const TCPConnection &) = delete;

                TCPConnection &operator=(const TCPConnection &) = delete;

                ~TCPConnection()
                {
                        if (f_socket.is_open()) f_socket.close();
                }

                static std::shared_ptr<TCPConnection> create(boost::asio::io_context &io_context)
                {
                        return std::make_shared<TCPConnection>(io_context);
                }

                boost::asio::ip::tcp::socket &socket();

                void open();

        private:
                void handle_read(const boost::system::error_code &error_code,
                                 const std::shared_ptr<std::array<char, MessageHeader::SIZE>> &header_buffer);

                void handle_add_order_request(const std::shared_ptr<std::vector<char>> &payload_buffer);

                void handle_write(const boost::system::error_code &ec)
                {
                        if (ec) {
                                std::cerr << ec.message() << std::endl;
                        } else {
                                open(); // Read next message
                        }
                }

                boost::asio::ip::tcp::socket f_socket;
        };

        inline boost::asio::ip::tcp::socket &TCPConnection::socket()
        {
                return f_socket;
        }

        inline void TCPConnection::open()
        {
                const auto self = shared_from_this();
                const auto header_buffer = std::make_shared<std::array<char, MessageHeader::SIZE>>();

                boost::asio::async_read(
                        f_socket,
                        boost::asio::buffer(*header_buffer),
                        [self, header_buffer](const boost::system::error_code ec, size_t) {
                                self->handle_read(ec, header_buffer);
                        }
                );
        }

        inline void TCPConnection::handle_read(
                const boost::system::error_code &error_code,
                const std::shared_ptr<std::array<char, MessageHeader::SIZE>> &header_buffer)
        {
                if (error_code) {
                        std::cerr << "Read error: " << error_code.message() << std::endl;
                        return;
                }

                auto [type, length] = MessageHeader::deserialize(header_buffer->data());

                const auto self = shared_from_this();
                auto payload_buffer = std::make_shared<std::vector<char>>(length);

                boost::asio::async_read(
                        f_socket,
                        boost::asio::buffer(*payload_buffer),
                        [self, type, payload_buffer](const boost::system::error_code &ec, size_t) {
                                if (ec) {
                                        std::cerr << "Read error: " << ec.message() << std::endl;
                                        return;
                                }

                                if (type == MessageType::AddOrderRequest) {
                                        self->handle_add_order_request(payload_buffer);
                                }
                                // You could dispatch other message types here
                        });
        }

        inline void TCPConnection::handle_add_order_request(const std::shared_ptr<std::vector<char>> &payload_buffer)
        {
                auto [symbol, price, quantity, side] = AddOrderRequest::deserialize(payload_buffer->data());

                std::cout << "received order: " <<
                        "symbol: " << symbol <<
                        ", price: " << price <<
                        ", quantity: " << quantity <<
                        ", side: " << (side == Side::Buy ? "BUY" : "SELL") <<
                        std::endl;

                constexpr size_t total_size = MessageHeader::SIZE + AddOrderResponse::SIZE;
                char response_buffer[total_size];

                MessageHeader::serialize({MessageType::AddOrderResponse, AddOrderResponse::SIZE}, response_buffer);

                const AddOrderResponse response{
                        12345,
                        static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count())
                };

                const auto self = shared_from_this();
                AddOrderResponse::serialize(response, response_buffer + MessageHeader::SIZE);

                boost::asio::async_write(
                        f_socket,
                        boost::asio::buffer(response_buffer, total_size),
                        [self](const boost::system::error_code ec, size_t) {
                                self->handle_write(ec);
                        }
                );
        }
}

#endif // TCP_CONNECTION_H
