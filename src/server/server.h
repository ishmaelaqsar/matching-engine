#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <memory>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

constexpr std::string DEFAULT_HOST = "localhost";
constexpr int DEFAULT_PORT = 8080;
constexpr int DEFAULT_BUFFER_SIZE = 1024;

namespace orderbook
{
        class Server
        {
        public:
                Server() :
                        Server(DEFAULT_HOST, DEFAULT_PORT, DEFAULT_BUFFER_SIZE) {}

                Server(const std::string &host, const int port) :
                        Server(host, port, DEFAULT_BUFFER_SIZE) {}

                Server(std::string host, const int port, const int buffer_size) :
                        f_host(std::move(host)),
                        f_port(port),
                        f_buffer(new char[buffer_size]) {}

                void init();

                void start();

        private:
                std::string f_host;
                int f_port;
                char *f_buffer;
                int f_server_fd{};
                int f_server_socket{};
        };

        void Server::init()
        {
                struct sockaddr_in server_address;

        }
}

#endif //SERVER_H
