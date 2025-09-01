#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <memory>

#include "core/ring_buffer.h"
#include "core/types.h"

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

                explicit Connection(core::ConnectionId id, boost::asio::io_context &io_context,
                                    boost::asio::ip::tcp::socket socket,
                                    const std::function<void(core::Payload)> &request_enqueue,
                                    const std::function<void(core::ConnectionId)> &connection_fail);

                core::ConnectionId id() const
                {
                        return f_id;
                }

                void start();

                void write(const core::protocol::Header &header, const core::Data &data);

        private:
                void handle_read(const boost::system::error_code &error_code);

                void handle_write(const boost::system::error_code &error_code,
                                  const std::shared_ptr<std::vector<unsigned char>> &data);

                const core::ConnectionId f_id;
                const std::function<void(core::Payload)> f_request_enqueue;
                const std::function<void(core::ConnectionId)> &f_connection_fail;
                boost::asio::ip::tcp::socket f_socket;
                boost::asio::io_context &f_io_context;
                std::array<unsigned char, core::protocol::Header::Size> f_header_buffer{};
                std::vector<unsigned char> f_payload_buffer{};
        };
} // namespace tcp
