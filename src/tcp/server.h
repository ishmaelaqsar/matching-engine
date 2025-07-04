#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>

#include "connection.h"

constexpr unsigned short DEFAULT_PORT = 8080;

namespace orderbook
{
        class TCPServer
        {
        public:
                explicit TCPServer(boost::asio::io_context &io_context) :
                        TCPServer(io_context, DEFAULT_PORT) {}

                TCPServer(boost::asio::io_context &io_context, const unsigned short &port) :
                        f_io_context(io_context),
                        f_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

                void start_accept();

        private:
                void handle_accept(const std::shared_ptr<TCPConnection> &connection,
                                   const boost::system::error_code &error_code);

                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::acceptor f_acceptor;
        };

        inline void TCPServer::start_accept()
        {
                const auto connection = TCPConnection::create(f_io_context);
                f_acceptor.async_accept(connection->socket(),
                                        [this, connection](const boost::system::error_code &ec) {
                                                handle_accept(connection, ec);
                                        });
        }

        inline void TCPServer::handle_accept(const std::shared_ptr<TCPConnection> &connection,
                                             const boost::system::error_code &error_code)
        {
                if (!error_code) {
                        connection->open();
                }

                start_accept(); // Accept next connection
        }
}

#endif //SERVER_H
