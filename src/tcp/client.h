#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <boost/asio.hpp>

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
                        f_socket(boost::asio::ip::tcp::socket(io_context)) {}

                void connect();

                void disconnect();

                [[nodiscard]] bool is_connected() const;

                explicit operator bool() const;

                common::AddOrderResponse add_order(const common::AddOrderRequest &request);

        private:
                common::AddOrderResponse add_order_response();

                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::socket f_socket;
                bool f_connected = false;
        };

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
                        f_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec); // NOLINT(*-unused-return-value)
                        if (ec) std::cerr << ec.message() << "\n";
                        f_socket.close(ec); // NOLINT(*-unused-return-value)
                        if (ec) std::cerr << ec.message() << "\n";
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

        inline common::AddOrderResponse Client::add_order(const common::AddOrderRequest &request)
        {
                if (!f_connected) throw std::runtime_error("Not connected");

                char buffer[common::MessageHeader::Size + common::AddOrderRequest::Size];
                common::MessageHeader::serialize({common::MessageType::AddOrderRequest, common::AddOrderRequest::Size}, buffer);
                common::AddOrderRequest::serialize(request, buffer + common::MessageHeader::Size);

                f_socket.send(boost::asio::buffer(buffer));

                return add_order_response();
        }

        inline common::AddOrderResponse Client::add_order_response()
        {
                std::array<char, common::MessageHeader::Size> header_buffer{};
                f_socket.receive(boost::asio::buffer(header_buffer));
                auto [type, length] = common::MessageHeader::deserialize(header_buffer.data());

                if (type != common::MessageType::AddOrderResponse) {
                        throw std::runtime_error("Unexpected message type");
                }

                if (length != common::AddOrderResponse::Size) {
                        throw std::runtime_error("Invalid response size");
                }

                char payload_buffer[common::AddOrderResponse::Size];
                f_socket.receive(boost::asio::buffer(payload_buffer, common::AddOrderResponse::Size));
                return common::AddOrderResponse::deserialize(payload_buffer);
        }
}

#endif //CLIENT_H
