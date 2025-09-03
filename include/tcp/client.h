#pragma once

#include <boost/asio.hpp>

#include <core/protocol/header.h>
#include <core/protocol/message.h>

namespace tcp
{
        class Client : public std::enable_shared_from_this<Client>
        {
        public:
                using ResponseHandler = std::function<void(std::shared_ptr<core::protocol::Message>)>;

                Client(boost::asio::io_context &io_context, const std::string &host, unsigned short port,
                       ResponseHandler response_handler);

                ~Client();

                void connect();

                void disconnect();

                void request(const core::protocol::Message &request);

        private:
                void read_header();
                void read_payload(const core::protocol::Header &header);

                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::ip::tcp::socket f_socket;

                std::array<unsigned char, core::protocol::Header::Size> f_header_buffer{};
                std::vector<unsigned char> f_payload_buffer{};

                const ResponseHandler f_response_handler;
        };
} // namespace tcp
