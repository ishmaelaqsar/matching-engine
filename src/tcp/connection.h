#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>

#include <memory>

#include "../common/protocol/trading/add_order.h"
#include "../common/types.h"
#include "handlers/trading.h"
#include "header.h"

namespace tcp
{
        class Connection : public std::enable_shared_from_this<Connection>
        {
        public:
                explicit Connection(boost::asio::io_context &io_context) : f_socket(io_context)
                {}

                Connection(const Connection &) = delete;

                Connection &operator=(const Connection &) = delete;

                ~Connection()
                {
                        if (f_socket.is_open()) f_socket.close();
                }

                static std::shared_ptr<Connection> create(boost::asio::io_context &io_context)
                {
                        return std::make_shared<Connection>(io_context);
                }

                boost::asio::ip::tcp::socket &socket()
                {
                        return f_socket;
                }

                void open();

        private:
                void handle_read(const boost::system::error_code &error_code,
                                 const std::shared_ptr<std::array<char, MessageHeader::Size>> &header_buffer);

                static void handle_read_result(const common::MessageType &type,
                                               const std::shared_ptr<std::vector<char>> &payload_buffer,
                                               const boost::system::error_code &error_code);

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

        inline void Connection::open()
        {
                const auto self = shared_from_this();
                const auto header_buffer = std::make_shared<std::array<char, MessageHeader::Size>>();

                boost::asio::async_read(f_socket, boost::asio::buffer(*header_buffer),
                                        [self, header_buffer](const boost::system::error_code ec, size_t) {
                                                self->handle_read(ec, header_buffer);
                                        });
        }

        inline void Connection::handle_read(const boost::system::error_code &error_code,
                                            const std::shared_ptr<std::array<char, MessageHeader::Size>> &header_buffer)
        {
                if (error_code) {
                        std::cerr << "Read error: " << error_code.message() << std::endl;
                        return;
                }

                auto [type, length] = MessageHeader::deserialize(header_buffer->data());

                const auto self = shared_from_this();
                auto payload_buffer = std::make_shared<std::vector<char>>(length);

                boost::asio::async_read(
                        f_socket, boost::asio::buffer(*payload_buffer),
                        [self, type, payload_buffer](const boost::system::error_code &ec, size_t) -> void {
                                self->handle_read_result(type, payload_buffer, ec);
                        });
        }

        inline void Connection::handle_read_result(const common::MessageType &type,
                                                   const std::shared_ptr<std::vector<char>> &payload_buffer,
                                                   const boost::system::error_code &error_code)
        {
                if (error_code) {
                        std::cerr << "Read error: " << error_code.message() << std::endl;
                        return;
                }

                const auto self = shared_from_this();

                switch (type) {
                        case common::MessageType::AddOrderRequest:
                                const auto request =
                                        common::protocol::trading::AddOrderRequest::deserialize(payload_buffer->data());
                                const auto response =
                                        handlers::TradingMessageHandler::handle_add_order_request(request);
                                const size_t total_size = MessageHeader::Size + response.size();
                                char response_buffer[total_size];
                                MessageHeader::serialize({common::MessageType::AddOrderResponse, response.size()},
                                                         response_buffer);
                                common::protocol::trading::AddOrderResponse::serialize(
                                        response, response_buffer + MessageHeader::Size);
                                boost::asio::async_write(self->socket(),
                                                         boost::asio::buffer(response_buffer, total_size),
                                                         [self](const boost::system::error_code ec, size_t) -> void {
                                                                 self->handle_write(ec);
                                                         });
                                break;
                        default: break;
                }
        }
} // namespace tcp

#endif // CONNECTION_H
