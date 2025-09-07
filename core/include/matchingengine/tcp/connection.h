#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <memory>

#include <matchingengine/ring_buffer.h>
#include <matchingengine/types.h>
#include "matchingengine/protocol/header.h"

namespace tcp
{
        class Server;
}

namespace tcp
{
        class Connection : public std::enable_shared_from_this<Connection>
        {
        public:
                Connection(const Connection &) = delete;
                Connection &operator=(const Connection &) = delete;

                Connection(core::ConnectionId id, boost::asio::io_context &io_context,
                           boost::asio::ip::tcp::socket socket,
                           const std::function<void(core::Payload)> &request_enqueue,
                           const std::function<void(core::ConnectionId)> &connection_fail);

                core::ConnectionId id() const;

                void start();

                void write(const core::protocol::Header &header, const core::Data &data);

        private:
                static void handle_write(const boost::system::error_code &error_code,
                                         const std::shared_ptr<std::vector<unsigned char>> &data);

                void handle_read(const boost::system::error_code &error_code);

                const core::ConnectionId f_id;
                const std::function<void(core::Payload)> f_request_enqueue;
                const std::function<void(core::ConnectionId)> &f_connection_fail;
                boost::asio::ip::tcp::socket f_socket;
                boost::asio::io_context &f_io_context;
                std::array<unsigned char, core::protocol::Header::Size> f_header_buffer{};
                std::vector<unsigned char> f_payload_buffer{};
        };
} // namespace tcp
