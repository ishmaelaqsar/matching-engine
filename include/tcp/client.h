#pragma once

#include <boost/asio.hpp>

#include "core/protocol/message.h"

namespace tcp
{
        class Client
        {
        public:
                Client(boost::asio::io_context &io_context, const std::string &host, unsigned short port,
                       const std::function<void(core::protocol::Message &)> &response_handler);

                ~Client();

                void connect();

                void disconnect();

                [[nodiscard]] bool is_connected() const
                {
                        return f_connected.load();
                }

                explicit operator bool() const
                {
                        return f_connected.load();
                }

                void request(const core::protocol::Message &request);

        private:
                void start_response_thread();
                std::unique_ptr<core::protocol::Message> poll_responses();

                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::socket f_socket;
                const std::function<void(core::protocol::Message &)> f_response_handler;
                std::thread f_response_thread;
                std::atomic_bool f_connected = false;
                std::array<unsigned char, core::protocol::Header::Size> f_header_buffer{};
                std::vector<unsigned char> f_payload_buffer{};
        };
} // namespace tcp
