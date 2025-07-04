#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace orderbook
{
        class TCPConnection : public std::enable_shared_from_this<TCPConnection>
        {
        public:
                static std::shared_ptr<TCPConnection> create(boost::asio::io_context &io_context)
                {
                        return std::make_shared<TCPConnection>(io_context);
                }

                explicit TCPConnection(boost::asio::io_context &io_context) :
                        f_socket(io_context) {}

                boost::asio::ip::tcp::socket &socket();

                void open();

        private:
                boost::asio::ip::tcp::socket f_socket;
        };

        inline boost::asio::ip::tcp::socket &TCPConnection::socket()
        {
                return f_socket;
        }

        inline void TCPConnection::open()
        {
                auto self = shared_from_this();
                boost::asio::async_write(f_socket, boost::asio::buffer("Hello, World!"),
                                         [self](const boost::system::error_code &ec, std::size_t) {
                                                 if (ec) {
                                                         std::cout << "Connection Failed: " << ec.message() <<
                                                                 std::endl;
                                                 } else {
                                                         std::cout << "Connection successful on " <<
                                                                 self->socket().remote_endpoint() << std::endl;
                                                 }
                                         });
        }
}

#endif //TCP_CONNECTION_H
