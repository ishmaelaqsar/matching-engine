#ifndef ENGINE_H
#define ENGINE_H

#include <boost/chrono.hpp>
#include <thread>
#include <unordered_map>

#include "book.h"
#include "common/protocol/message.h"
#include "common/protocol/trading/add_order.h"
#include "common/protocol/trading/trade.h"
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
                                if (!f_inbound_buffer->pop(f_inbound_payload)) {
                                        return work_done;
                                }

                                BOOST_LOG_TRIVIAL(debug)
                                        << "Received request from connection ID=" << f_inbound_payload.connectionId;

                                const auto timestamp =
                                        static_cast<common::Timestamp>(now.time_since_epoch().count() + i);

                                switch (f_inbound_payload.header.type) {
                                        case common::protocol::MessageType::GetBookRequest: {
                                                handle_get_book_request();
                                                break;
                                        }
                                        case common::protocol::MessageType::AddOrderRequest: {
                                                handle_add_order_request(timestamp);
                                                break;
                                        }
                                        default: {
                                                BOOST_LOG_TRIVIAL(warning)
                                                        << "Unhandled message type="
                                                        << static_cast<int>(f_inbound_payload.header.type);
                                                break;
                                        }
                                }

                                work_done++;
                        }
                        return work_done;
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

                void handle_get_book_request()
                {
                        const auto request = deserialize<common::protocol::view::GetBookRequest>();
                        const auto &book = get_book(request.symbol());
                        auto [bids, asks] = book.snapshot();

                        serialize_and_enqueue(
                                common::protocol::view::GetBookResponse(std::move(bids), std::move(asks)));
                }

                void handle_add_order_request(const common::Timestamp timestamp)
                {
                        const auto request = deserialize<common::protocol::trading::AddOrderRequest>();
                        auto &book = get_book(request.symbol());

                        auto [order_id, trades] =
                                book.add_order(request.price(), request.quantity(), request.side(), timestamp);

                        serialize_and_enqueue(common::protocol::trading::AddOrderResponse(order_id, timestamp));

                        for (const auto &trade: trades) {
                                serialize_and_enqueue(common::protocol::trading::Trade(
                                        trade.id(), trade.price(), trade.quantity(), trade.timestamp(),
                                        trade.source_order(), trade.matched_order()));
                        }
                }

                template<typename T>
                T deserialize()
                {
                        auto t = T{};
                        t.deserialize(f_inbound_payload.data.data());
                        return t;
                }

                void serialize_and_enqueue(const common::protocol::Message &response)
                {
                        f_outbound_payload.connectionId = f_inbound_payload.connectionId;
                        f_outbound_payload.header = {response.type(), response.size()};
                        response.serialize(f_outbound_payload.data.data());
                        if (!f_outbound_buffer->push(f_outbound_payload)) {
                                BOOST_LOG_TRIVIAL(error) << "Outbound queue full, dropping message.";
                        }
                }

                static constexpr int BatchSize = 500;

                std::unordered_map<common::Symbol, std::unique_ptr<Book>> f_order_books{};
                common::Payload f_inbound_payload{};
                common::Payload f_outbound_payload{};
                const std::shared_ptr<common::RingBuffer<common::Payload>> &f_inbound_buffer;
                const std::shared_ptr<common::RingBuffer<common::Payload>> &f_outbound_buffer;
        };
} // namespace orderbook

#endif // ENGINE_H
