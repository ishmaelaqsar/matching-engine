#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <boost/asio.hpp>

#include "protocol/header.h"
#include "../common/add_order.h"

// NOT THREAD SAFE
namespace tcp
{
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

                AddOrderResponse add_order(const AddOrderRequest &request);

        private:
                AddOrderResponse add_order_response();

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

        inline AddOrderResponse Client::add_order(const AddOrderRequest &request)
        {
                if (!f_connected) throw std::runtime_error("Not connected");

                char buffer[MessageHeader::SIZE + AddOrderRequest::SIZE];
                MessageHeader::serialize({MessageType::AddOrderRequest, AddOrderRequest::SIZE}, buffer);
                AddOrderRequest::serialize(request, buffer + MessageHeader::SIZE);

                f_socket.send(boost::asio::buffer(buffer));

                return add_order_response();
        }

        inline AddOrderResponse Client::add_order_response()
        {
                std::array<char, MessageHeader::SIZE> header_buffer{};
                f_socket.receive(boost::asio::buffer(header_buffer));
                auto [type, length] = MessageHeader::deserialize(header_buffer.data());

                if (type != MessageType::AddOrderResponse) {
                        throw std::runtime_error("Unexpected message type");
                }

                if (length != AddOrderResponse::SIZE) {
                        throw std::runtime_error("Invalid response size");
                }

                char payload_buffer[AddOrderResponse::SIZE];
                f_socket.receive(boost::asio::buffer(payload_buffer, AddOrderResponse::SIZE));
                return AddOrderResponse::deserialize(payload_buffer);
        }
}

#endif //CLIENT_H
