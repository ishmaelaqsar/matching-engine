#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <matchingengine/protocol/header.h>
#include <matchingengine/ring_buffer.h>
#include <matchingengine/tcp/util.h>
#include <matchingengine/types.h>
#include <memory>

namespace tcp
{
    class Connection : public std::enable_shared_from_this<Connection>
    {
    public:
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        Connection(
            core::ConnectionId id, boost::asio::io_context& io_context,
            boost::asio::ip::tcp::socket socket,
            std::function<bool(const std::string&)> username_check,
            std::function<void(core::Payload)> request_enqueue,
            std::function<void(Command)> command_enqueue
        );

        core::ConnectionId id() const;
        std::string_view username() const;
        void start();
        void stop();
        void
        write(const core::protocol::Header& header, const core::Data& data);

    private:
        static void handle_write(
            const boost::system::error_code& error_code,
            const std::shared_ptr<std::vector<unsigned char>>& data
        );

        void read_header();
        void read_payload(core::protocol::Header header);
        void error(const boost::system::error_code& ec) const;
        void error(std::string&& message) const;

        const core::ConnectionId f_id;
        const std::function<bool(std::string&)> f_username_check;
        const std::function<void(core::Payload)> f_request_enqueue;
        const std::function<void(Command)> f_command_enqueue;

        std::atomic<bool> f_stopped{false};
        boost::asio::ip::tcp::socket f_socket;
        boost::asio::io_context& f_io_context;
        std::string f_username{};

        std::array<unsigned char, core::protocol::Header::Size> f_header_buffer{
        };
        std::vector<unsigned char> f_payload_buffer{};
    };
} // namespace tcp
