#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "tcp_connection.h"

constexpr short DEFAULT_PORT = 8080;

namespace orderbook
{
        class TCPServer
        {
        public:
                explicit TCPServer(boost::asio::io_context &io_context) :
                        TCPServer(io_context, DEFAULT_PORT) {}

                TCPServer(boost::asio::io_context &io_context, const short port) :
                        f_io_context(io_context),
                        f_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

                void start_accept();

                void handle_accept(const std::shared_ptr<TCPConnection> &connection,
                                   const boost::system::error_code &error_code);

        private:
                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::acceptor f_acceptor;
        };

        inline void TCPServer::start_accept()
        {
                const std::shared_ptr<TCPConnection> tcp_connection = TCPConnection::create(f_io_context);
                f_acceptor.async_accept(tcp_connection->socket(),
                                        boost::bind(&TCPServer::handle_accept, this, tcp_connection,
                                                    boost::asio::placeholders::error));
        }

        inline void TCPServer::handle_accept(const std::shared_ptr<TCPConnection> &connection,
                                             const boost::system::error_code &error_code)
        {
                if (!error_code) {
                        connection->start();
                }

                start_accept();
        }
}

#endif //SERVER_H
