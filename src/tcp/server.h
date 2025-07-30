#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind.hpp>

#include "connection.h"

constexpr unsigned short DefaultPort = 8080;

namespace tcp
{
        class Server
        {
        public:
                explicit Server(boost::asio::io_context &io_context) :
                        Server(io_context, DefaultPort) {}

                Server(boost::asio::io_context &io_context, const unsigned short &port) :
                        f_io_context(io_context),
                        f_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

                void start_accept();

        private:
                void handle_accept(const boost::system::error_code &error_code,
                                   const std::shared_ptr<Connection> &connection);

                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::acceptor f_acceptor;
        };

        inline void Server::start_accept()
        {
                const auto connection = Connection::create(f_io_context);
                f_acceptor.async_accept(connection->socket(),
                                        [this, connection](boost::system::error_code ec) {
                                                handle_accept(ec, connection);
                                        });
        }

        inline void Server::handle_accept(const boost::system::error_code &error_code,
                                          const std::shared_ptr<Connection> &connection)
        {
                if (!error_code) {
                        connection->open();
                } else {
                        std::cerr << error_code.message() << std::endl;
                }

                start_accept(); // Accept next connection
        }
}

#endif //SERVER_H
