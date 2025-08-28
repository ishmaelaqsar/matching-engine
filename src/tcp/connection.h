#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/log/trivial.hpp>

#include <memory>
#include <utility>

#include "../common/types.h"
#include "common/ring_buffer.h"

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

                explicit Connection(const common::ConnectionId id, boost::asio::io_context &io_context,
                                    boost::asio::ip::tcp::socket socket,
                                    const std::function<void(common::Payload)> &request_enqueue,
                                    const std::function<void(common::ConnectionId)> &connection_fail) :
                    f_id(id),
                    f_request_enqueue(request_enqueue),
                    f_connection_fail(connection_fail),
                    f_socket(std::move(socket)),
                    f_io_context(io_context)
                {}

                common::ConnectionId id() const
                {
                        return f_id;
                }

                void start()
                {
                        boost::asio::async_read(
                                f_socket, boost::asio::buffer(f_header_buffer),
                                [self = shared_from_this()](const boost::system::error_code ec, const size_t) -> void {
                                        self->handle_read(ec);
                                });
                }

                void write(const common::protocol::Header &header, const common::Data &data)
                {
                        const size_t total_size = common::protocol::Header::Size + header.length;
                        const auto buffer = std::make_shared<std::vector<unsigned char>>(total_size);

                        common::protocol::Header::serialize(header, buffer->data());
                        std::memcpy(buffer->data() + common::protocol::Header::Size, data.data(), header.length);

                        boost::asio::async_write(f_socket, boost::asio::buffer(*buffer, total_size),
                                                 [self = shared_from_this(), buffer](const boost::system::error_code ec,
                                                                                     size_t) -> void {
                                                         self->handle_write(ec, buffer);
                                                 });
                }

        private:
                void handle_read(const boost::system::error_code &error_code)
                {
                        if (error_code) {
                                BOOST_LOG_TRIVIAL(error) << "Read error: " << error_code.message();
                                return;
                        }

                        const auto header = common::protocol::Header::deserialize(f_header_buffer.data());
                        f_payload_buffer.resize(header.length);

                        boost::asio::async_read(
                                f_socket, boost::asio::buffer(f_payload_buffer.data(), header.length),
                                [self = shared_from_this(), header](const boost::system::error_code &ec,
                                                                    const size_t) -> void {
                                        if (ec) {
                                                BOOST_LOG_TRIVIAL(error) << "Read error: " << ec.message();
                                                self->f_connection_fail(self->f_id);
                                                return;
                                        }
                                        auto data = common::Data{};
                                        if (common::MaxPayloadSize < header.length) {
                                                BOOST_LOG_TRIVIAL(warning)
                                                        << "Request size is too big, dropping message.";
                                        } else {
                                                std::memcpy(data.data(), self->f_payload_buffer.data(), header.length);
                                                const auto payload = common::Payload{self->f_id, header, data};
                                                self->f_request_enqueue(payload);
                                        }
                                        self->start();
                                });
                }

                void handle_write(const boost::system::error_code &error_code,
                                  const std::shared_ptr<std::vector<unsigned char>> &data)
                {
                        if (error_code) {
                                BOOST_LOG_TRIVIAL(error) << "Write error: " << error_code.message();
                        } else {
                                BOOST_LOG_TRIVIAL(debug) << "Write success, " << data->size() << " bytes.";
                        }
                }

                const common::ConnectionId f_id;
                const std::function<void(common::Payload)> f_request_enqueue;
                const std::function<void(common::ConnectionId)> &f_connection_fail;
                boost::asio::ip::tcp::socket f_socket;
                boost::asio::io_context &f_io_context;
                std::array<unsigned char, common::protocol::Header::Size> f_header_buffer{};
                std::vector<unsigned char> f_payload_buffer{};
        };
} // namespace tcp

#endif // CONNECTION_H
