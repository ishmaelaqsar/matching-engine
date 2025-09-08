#pragma once

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include <matchingengine/protocol/header.h>
#include <matchingengine/protocol/message.h>
#include <matchingengine/protocol/view/get_book.h>

namespace tcp
{
        class Client : public std::enable_shared_from_this<Client>
        {
        public:
                template<typename SubMessage>
                using ResponseHandler = std::function<void(const SubMessage &)>;

                Client(boost::asio::io_context &io_context, const std::string &host, unsigned short port);

                ~Client();

                void connect();

                void disconnect();

                template<typename SubMessage>
                void register_handler(ResponseHandler<SubMessage> handler);

                template<typename SubMessage>
                void request(const SubMessage &request);

        private:
                void read_header();
                void read_payload(const core::protocol::Header &header);

                boost::asio::io_context &f_io_context;
                boost::asio::ip::tcp::endpoint f_endpoint;
                boost::asio::ip::tcp::socket f_socket;

                std::array<unsigned char, core::protocol::Header::Size> f_header_buffer{};
                std::vector<unsigned char> f_payload_buffer{};

                std::unordered_map<core::protocol::MessageType, std::function<void(const std::vector<unsigned char> &)>>
                        f_handlers;
        };

        template<typename SubMessage>
        void Client::register_handler(ResponseHandler<SubMessage> handler)
        {
                static_assert(std::is_base_of_v<core::protocol::Message<SubMessage>, SubMessage>,
                              "SubMessage must inherit from Message<SubMessage>");

                f_handlers[SubMessage{}.type()] = [h = std::move(handler)](const std::vector<unsigned char> &buffer) {
                        SubMessage msg{};
                        msg.deserialize(buffer.data());
                        h(msg);
                };
        }

        template<typename SubMessage>
        void Client::request(const SubMessage &request)
        {
                static_assert(std::is_base_of_v<core::protocol::Message<SubMessage>, SubMessage>,
                              "SubMessage must derive from Message<SubMessage>");

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
} // namespace tcp
