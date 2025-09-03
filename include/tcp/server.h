#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <thread>

#include <core/ring_buffer.h>
#include <core/types.h>
#include <tcp/connection.h>

namespace tcp
{
        class Server : public std::enable_shared_from_this<Server>
        {
        public:
                Server(const std::shared_ptr<core::RingBuffer<core::Payload>> &inbound_buffer,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &outbound_buffer,
                       boost::asio::io_context &io_context);

                Server(const unsigned short &port,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &inbound_buffer,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &outbound_buffer,
                       boost::asio::io_context &io_context);

                void start();

                void stop();

                bool is_running() const;

                void queue_request(const core::Payload &request) const;

                void queue_response(const core::Payload &response) const;

        private:
                void accept_connections();

                void poll_responses() const;

                core::ConnectionId f_counter = 0;
                std::atomic<bool> f_running{false};
                std::thread f_polling_thread;
                std::unordered_map<core::ConnectionId, std::shared_ptr<Connection>> f_connections;
                boost::asio::ip::tcp::acceptor f_acceptor;
                boost::asio::io_context &f_io_context;

                const std::shared_ptr<core::RingBuffer<core::Payload>> &f_inbound_buffer;
                const std::shared_ptr<core::RingBuffer<core::Payload>> &f_outbound_buffer;
        };
} // namespace tcp
