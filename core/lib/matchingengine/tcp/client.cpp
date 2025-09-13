#include <matchingengine/tcp/client.h>

#include <boost/log/trivial.hpp>

#include <matchingengine/protocol/header.h>

namespace tcp
{
        Client::Client(boost::asio::io_context &io_context, const std::string &host, const unsigned short port) :
            f_io_context(io_context),
            f_endpoint({boost::asio::ip::make_address(host), port}),
            f_socket(boost::asio::ip::tcp::socket(io_context))
        {}

        Client::~Client()
        {
                disconnect();
        }

        void Client::connect(std::string username)
        {
                BOOST_LOG_TRIVIAL(info) << "Connecting to server at " << f_endpoint;
                f_socket.async_connect(f_endpoint, [self = shared_from_this()](const boost::system::error_code &ec) {
                        if (ec) {
                                BOOST_LOG_TRIVIAL(error) << "Connect failed: " << ec.message();
                                return;
                        }
                        BOOST_LOG_TRIVIAL(info) << "Connection succeeded";
                        self->read_header();
                });
        }

        void Client::disconnect()
        {
                boost::system::error_code ec;
                f_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                f_socket.close(ec);
                if (ec) BOOST_LOG_TRIVIAL(error) << "Socket close error: " << ec.message();
        }

        void Client::read_header()
        {
                boost::asio::async_read(
                        f_socket, boost::asio::buffer(f_header_buffer),
                        [self = shared_from_this()](const boost::system::error_code &ec,
                                                    const std::size_t bytes) -> void {
                                BOOST_LOG_TRIVIAL(info) << "Header read completed,  " << bytes << " bytes read";

                                if (ec) {
                                        BOOST_LOG_TRIVIAL(error) << "Header read error: " << ec.message();
                                        self->disconnect();
                                        return;
                                }

                                const auto header = core::protocol::Header::deserialize(self->f_header_buffer.data());
                                BOOST_LOG_TRIVIAL(debug) << "Header received: type=" << static_cast<int>(header.type)
                                                         << " length=" << header.length;

                                self->read_payload(header);
                        });
        }

        void Client::read_payload(const core::protocol::Header &header)
        {
                f_payload_buffer.resize(header.length);

                boost::asio::async_read(
                        f_socket, boost::asio::buffer(f_payload_buffer),
                        [self = shared_from_this(), header](const boost::system::error_code &ec,
                                                            const std::size_t bytes) -> void {
                                BOOST_LOG_TRIVIAL(info) << "Payload read completed,  " << bytes << " bytes read";

                                if (ec) {
                                        BOOST_LOG_TRIVIAL(error) << "Payload read error: " << ec.message();
                                        self->disconnect();
                                        return;
                                }

                                try {
                                        BOOST_LOG_TRIVIAL(debug)
                                                << "Trying to deserialize message of type: " << header.type;

                                        if (const auto it = self->f_handlers.find(header.type);
                                            it == self->f_handlers.end()) {
                                                BOOST_LOG_TRIVIAL(warning)
                                                        << "No handlers for header type: " << header.type;
                                        } else {
                                                it->second(self->f_payload_buffer);
                                        }
                                } catch (const std::exception &e) {
                                        BOOST_LOG_TRIVIAL(error) << "Deserialize or Handler error: " << e.what();
                                } catch (...) {
                                        BOOST_LOG_TRIVIAL(error) << "Deserialize or Handler error: unknown";
                                }

                                self->read_header();
                        });
        }
} // namespace tcp
