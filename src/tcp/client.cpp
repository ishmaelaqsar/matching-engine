#include <boost/log/trivial.hpp>

#include <core/protocol/header.h>
#include <tcp/client.h>

namespace tcp
{
        Client::Client(boost::asio::io_context &io_context, const std::string &host, const unsigned short port,
                       ResponseHandler response_handler) :
            f_io_context(io_context),
            f_endpoint({boost::asio::ip::make_address(host), port}),
            f_socket(boost::asio::ip::tcp::socket(io_context)),
            f_response_handler(std::move(response_handler))
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

        void Client::request(const core::protocol::Message &request)
        {
                if (!f_socket.is_open()) {
                        BOOST_LOG_TRIVIAL(error) << "Request: socket not open";
                        return;
                }

                const auto buffer_size = core::protocol::Header::Size + request.size();
                auto buffer = std::make_shared<std::vector<unsigned char>>(buffer_size);

                core::protocol::Header::serialize({request.type(), request.size()}, buffer->data());
                request.serialize(buffer->data() + core::protocol::Header::Size);

                BOOST_LOG_TRIVIAL(debug) << "async_write: scheduling write of " << buffer_size << " bytes";
                boost::asio::async_write(
                        f_socket, boost::asio::buffer(*buffer),
                        [self = shared_from_this()](const boost::system::error_code &ec, const std::size_t bytes) {
                                if (ec) {
                                        BOOST_LOG_TRIVIAL(error) << "async_write error: " << ec.message();
                                } else {
                                        BOOST_LOG_TRIVIAL(info) << "async_write completed, wrote " << bytes << " bytes";
                                }
                        });
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
                                        const auto response = core::protocol::create(header.type);
                                        response->deserialize(self->f_payload_buffer.data());

                                        BOOST_LOG_TRIVIAL(debug) << "Received: " << *response;
                                        // self->f_response_handler(response);
                                } catch (const std::exception &e) {
                                        BOOST_LOG_TRIVIAL(error) << "Deserialize or Handler error: " << e.what();
                                } catch (...) {
                                        BOOST_LOG_TRIVIAL(error) << "Deserialize or Handler error: unknown";
                                }

                                self->read_header();
                        });
        }
} // namespace tcp
