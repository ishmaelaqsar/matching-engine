#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bimap.hpp>
#include <boost/circular_buffer.hpp>
#include <matchingengine/ring_buffer.h>
#include <matchingengine/tcp/connection.h>
#include <matchingengine/types.h>

namespace tcp
{
    class Server : public std::enable_shared_from_this<Server>
    {
    public:
        Server(
            core::RingBuffer<core::Payload>* inbound_buffer,
            core::RingBuffer<core::Payload>* outbound_buffer,
            boost::asio::io_context& io_context
        );

        Server(
            const unsigned short& port,
            core::RingBuffer<core::Payload>* inbound_buffer,
            core::RingBuffer<core::Payload>* outbound_buffer,
            boost::asio::io_context& io_context
        );

        void start();

        void stop();

        bool is_running() const;

    private:
        void accept_connections();

        void schedule_poll();

        void poll_responses();

        void send_message(
            const core::ConnectionId& id, const core::protocol::Header& header,
            const core::Data& data
        );

        static constexpr auto MaxCommands = 64;

        core::ConnectionId f_counter = 0;
        bool f_running = false;
        boost::asio::ip::tcp::acceptor f_acceptor;
        boost::asio::io_context& f_io_context;
        boost::asio::steady_timer f_timer;

        boost::bimap<core::ConnectionId, core::Username> f_active_connections{};
        std::unordered_map<core::ConnectionId, std::shared_ptr<Connection>>
            f_connections_by_id{};

        boost::circular_buffer<Command> f_command_queue{MaxCommands};
        core::RingBuffer<core::Payload>* f_inbound_buffer;
        core::RingBuffer<core::Payload>* f_outbound_buffer;
    };
} // namespace tcp
