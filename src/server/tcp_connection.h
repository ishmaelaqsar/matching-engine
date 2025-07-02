#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind/bind.hpp>

namespace orderbook
{
        class TCPConnection
        {
        public:
                static std::shared_ptr<TCPConnection> create(boost::asio::io_context &io_context)
                {
                        return std::make_shared<TCPConnection>(TCPConnection(io_context));
                }

                boost::asio::ip::tcp::socket &socket();

                void start();

        private:
                explicit TCPConnection(boost::asio::io_context &io_context) :
                        f_socket(io_context) {}

                void handle_write(const boost::system::error_code & /*error*/,
                                  size_t /*bytes_transferred*/) {}

                boost::asio::ip::tcp::socket f_socket;
        };

        inline boost::asio::ip::tcp::socket &TCPConnection::socket()
        {
                return f_socket;
        }

        inline void TCPConnection::start()
        {
                boost::asio::async_write(f_socket, boost::asio::buffer("Hello, World!"),
                                         boost::bind(&TCPConnection::handle_write, this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred));
        }
}

#endif //TCP_CONNECTION_H
