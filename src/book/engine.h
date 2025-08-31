#ifndef ENGINE_H
#define ENGINE_H

#include <boost/chrono.hpp>
#include <thread>
#include <unordered_map>

#include "book.h"
#include "common/protocol/trading/add_order.h"
#include "common/protocol/view/get_book.h"
#include "common/ring_buffer.h"
#include "common/types.h"

namespace orderbook
{
        class Engine
        {
        public:
                Engine(const std::shared_ptr<common::RingBuffer<common::Payload>> &inbound_buffer,
                       const std::shared_ptr<common::RingBuffer<common::Payload>> &outbound_buffer) :
                    f_inbound_buffer(inbound_buffer), f_outbound_buffer(outbound_buffer)
                {}

                int do_work()
                {
                        const auto now = boost::chrono::system_clock::now();
                        int work_done = 0;
                        for (int i = 0; i < BatchSize; i++) {
                                if (f_inbound_buffer->pop(f_inbound_payload)) {
                                        BOOST_LOG_TRIVIAL(debug) << "Received request from connection ID="
                                                                 << f_inbound_payload.connectionId;
                                        switch (f_inbound_payload.header.type) {
                                                case common::MessageType::GetBookRequest: {
                                                        const auto request =
                                                                deserialize<common::protocol::view::GetBookRequest>();
                                                        const auto book = get_book(request.symbol());
                                                        auto [bids, asks] = book.snapshot();

                                                        serialize_and_enqueue(common::protocol::view::GetBookResponse(
                                                                std::move(bids), std::move(asks)));
                                                }
                                                case common::MessageType::AddOrderRequest: {
                                                        const auto request = deserialize<
                                                                common::protocol::trading::AddOrderRequest>();
                                                        const auto timestamp = now.time_since_epoch().count() + i;
                                                        auto book = get_book(request.symbol());

                                                        auto [order_id, trades] =
                                                                book.add_order(request.price(), request.quantity(),
                                                                               request.side(), timestamp);

                                                        serialize_and_enqueue(
                                                                common::protocol::trading::AddOrderResponse(order_id,
                                                                                                            timestamp));
                                                }
                                                default: break;
                                        }
                                        work_done++;
                                } else {
                                        return work_done;
                                }
                        }
                }

        private:
                Book &get_book(const common::Symbol &symbol)
                {
                        auto &book = f_order_books[symbol];
                        if (!book) {
                                book = std::make_unique<Book>();
                        }
                        return *book;
                }

                template<typename T>
                T deserialize()
                {
                        auto t = T{};
                        t.deserialize(f_inbound_payload.data.data());
                        return t;
                }

                template<typename R>
                void serialize_and_enqueue(const R &response)
                {
                        f_outbound_payload.connectionId = f_inbound_payload.connectionId;
                        f_outbound_payload.header = {response.type(), response.size()};
                        response.serialize(f_outbound_payload.data.data());
                        if (!f_inbound_buffer->push(f_outbound_payload)) {
                                BOOST_LOG_TRIVIAL(error) << "Outbound queue full, dropping message.";
                        }
                }

                static constexpr int BatchSize = 500;

                std::unordered_map<common::Symbol, std::unique_ptr<Book>> f_order_books{};
                std::vector<unsigned char> f_payload_buffer{};
                common::Payload f_inbound_payload{};
                common::Payload f_outbound_payload{};
                const std::shared_ptr<common::RingBuffer<common::Payload>> &f_inbound_buffer;
                const std::shared_ptr<common::RingBuffer<common::Payload>> &f_outbound_buffer;
        };
} // namespace orderbook

#endif // ENGINE_H
