#include <boost/log/trivial.hpp>

#include "src/tcp/server.h"

#include "common/protocol/trading/add_order.h"

int main()
{
        const auto inbound = std::make_shared<common::RingBuffer<common::Payload>>();
        const auto outbound = std::make_shared<common::RingBuffer<common::Payload>>();
        try {
                boost::asio::io_context io_context;
                const auto server = std::make_shared<tcp::Server>(inbound, outbound, io_context);
                server->start();

                std::thread domain([&]() -> void {
                        while (server->is_running()) {
                                try {
                                        if (common::Payload payload{}; inbound->pop(payload)) {
                                                BOOST_LOG_TRIVIAL(info)
                                                        << "Received payload from connection " << payload.connectionId;
                                                switch (payload.header.type) {
                                                        case common::MessageType::AddOrderRequest: {
                                                                auto request =
                                                                        common::protocol::trading::AddOrderRequest{};
                                                                request.deserialize(payload.data.data());
                                                                BOOST_LOG_TRIVIAL(info) << request;
                                                        }
                                                        default: break;
                                                }
                                        } else {
                                                std::this_thread::yield();
                                        }
                                } catch (const std::exception &e) {
                                        BOOST_LOG_TRIVIAL(error) << "Domain Thread: " << e.what();
                                } catch (...) {
                                        BOOST_LOG_TRIVIAL(error) << "Domain Thread: unknown exception";
                                }
                        }
                });

                try {
                        io_context.run();
                } catch (const std::exception &e) {
                        BOOST_LOG_TRIVIAL(error) << "io_context exception: " << e.what();
                } catch (...) {
                        BOOST_LOG_TRIVIAL(error) << "io_context: unknown exception";
                }

                domain.join();
        } catch (const std::exception &e) {
                BOOST_LOG_TRIVIAL(error) << "Main exception: " << e.what();
                return 1;
        } catch (...) {
                BOOST_LOG_TRIVIAL(error) << "Main exception: unknown exception";
                return 1;
        }

        return 0;
}
