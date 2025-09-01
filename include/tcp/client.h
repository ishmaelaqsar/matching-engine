#pragma once

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include "core/protocol/message.h"

namespace tcp
{
        // NOT THREAD SAFE
        class Client
        {
        public:
                Client(boost::asio::io_context &io_context, const std::string &host, const unsigned short port) :
                    f_endpoint({boost::asio::ip::make_address(host), port}),
                    f_io_context(io_context),
                    f_socket(boost::asio::ip::tcp::socket(io_context))
                {}

                ~Client()
                {
                        disconnect();
                }

                void connect();

                void disconnect();

                [[nodiscard]] bool is_connected() const;

                explicit operator bool() const;

                std::shared_ptr<core::protocol::Message>
                request(const std::unique_ptr<core::protocol::Message> &request);

        private:
                std::shared_ptr<core::protocol::Message> response(core::protocol::MessageType requestType);

                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::socket f_socket;
                bool f_connected = false;
        };
} // namespace tcp
