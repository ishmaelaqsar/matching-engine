#include <boost/log/trivial.hpp>

#include "core/protocol/header.h"
#include "tcp/client.h"

namespace tcp
{
        Client::Client(boost::asio::io_context &io_context, const std::string &host, const unsigned short port,
                       const std::function<void(core::protocol::Message &)> &response_handler) :
            f_endpoint({boost::asio::ip::make_address(host), port}),
            f_io_context(io_context),
            f_socket(boost::asio::ip::tcp::socket(io_context)),
            f_response_handler(response_handler)
        {
                if (!f_response_handler) {
                        throw std::invalid_argument("Invalid response handler");
                }
        }

        Client::~Client()
        {
                disconnect();
        }

        void Client::connect()
        {
                BOOST_LOG_TRIVIAL(info) << "Connecting to server at " << f_endpoint;
                try {
                        f_socket.connect(f_endpoint);
                        f_connected.store(true);
                        BOOST_LOG_TRIVIAL(info) << "Connection succeeded";
                } catch (const boost::system::system_error &e) {
                        f_connected.store(false);
                        throw std::runtime_error("Connection failed: " + std::string(e.what()));
                }

                start_response_thread();
        }

        void Client::disconnect()
        {
                if (f_connected.load()) {
                        BOOST_LOG_TRIVIAL(info) << "Disconnecting from server";
                        boost::system::error_code ec;
                        f_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                        if (ec) BOOST_LOG_TRIVIAL(error) << ec.message();
                        f_socket.close(ec);
                        if (ec) BOOST_LOG_TRIVIAL(error) << ec.message();
                        f_connected.store(false);
                        if (f_response_thread.joinable()) {
                                f_response_thread.join();
                        }
                }
        }

        void Client::request(const core::protocol::Message &request)
        {
                if (!f_connected.load()) throw std::runtime_error("Not connected");

                const size_t buffer_size = core::protocol::Header::Size + request.size();
                std::vector<unsigned char> buffer(buffer_size);

                core::protocol::Header::serialize({request.type(), request.size()}, buffer.data());
                request.serialize(buffer.data() + core::protocol::Header::Size);

                f_socket.send(boost::asio::buffer(buffer));
        }

        void Client::start_response_thread()
        {
                BOOST_LOG_TRIVIAL(info) << "Starting response handler thread";

                f_response_thread = std::thread([this]() -> void {
                        while (f_connected.load()) {
                                BOOST_LOG_TRIVIAL(info) << "Waiting for responses...";
                                try {
                                        if (const auto response = poll_responses()) {
                                                f_response_handler(*response);
                                        } else {
                                                std::this_thread::yield();
                                        }
                                } catch (const std::exception &e) {
                                        BOOST_LOG_TRIVIAL(error) << "Response Thread: " << e.what();
                                } catch (...) {
                                        BOOST_LOG_TRIVIAL(error) << "Response Thread: unknown exception";
                                }
                        }
                });
        }

        std::unique_ptr<core::protocol::Message> Client::poll_responses()
        {
                boost::asio::async_read(
                        f_socket, boost::asio::buffer(f_header_buffer),
                        [this](const boost::system::error_code ec, std::size_t) {
                                if (ec) {
                                        BOOST_LOG_TRIVIAL(error) << "Header read error: " << ec.message();
                                        disconnect();
                                        return;
                                }
                                auto [type, length] = core::protocol::Header::deserialize(f_header_buffer.data());
                                f_payload_buffer.resize(length);
                                // Start reading payload
                                boost::asio::async_read(
                                        f_socket, boost::asio::buffer(f_payload_buffer),
                                        [this, type](boost::system::error_code ec, std::size_t /*length*/) {
                                                if (ec) {
                                                        BOOST_LOG_TRIVIAL(error)
                                                                << "Payload read error: " << ec.message();
                                                        disconnect();
                                                        return;
                                                }

                                                const auto response = core::protocol::create(type);
                                                response->deserialize(f_payload_buffer.data());

                                                // Call user handler
                                                if (f_response_handler) f_response_handler(*response);

                                                // Continue reading next message
                                                poll_responses();
                                        });
                        });
        }
} // namespace tcp
