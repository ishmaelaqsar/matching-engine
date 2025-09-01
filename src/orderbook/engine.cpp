#include <boost/chrono.hpp>
#include <boost/log/trivial.hpp>

#include "core/protocol/message.h"
#include "core/protocol/trading/add_order.h"
#include "core/protocol/trading/trade.h"
#include "core/protocol/view/get_book.h"
#include "orderbook/engine.h"

namespace orderbook
{
        static constexpr auto BatchSize = 500;

        Engine::Engine(const std::shared_ptr<core::RingBuffer<core::Payload>> &inbound_buffer,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &outbound_buffer) :
            f_inbound_buffer(inbound_buffer), f_outbound_buffer(outbound_buffer)
        {}

        int Engine::do_work()
        {
                int work_done = 0;
                for (int i = 0; i < BatchSize; i++) {
                        core::Payload inbound_payload{};
                        if (!f_inbound_buffer->pop(inbound_payload)) {
                                return work_done;
                        }

                        BOOST_LOG_TRIVIAL(debug)
                                << "Received request from connection ID=" << inbound_payload.connectionId;

                        const auto timestamp = static_cast<core::Timestamp>(
                                boost::chrono::system_clock::now().time_since_epoch().count() + i);

                        switch (inbound_payload.header.type) {
                                case core::protocol::MessageType::GetBookRequest: {
                                        handle_get_book_request(inbound_payload);
                                        break;
                                }
                                case core::protocol::MessageType::AddOrderRequest: {
                                        handle_add_order_request(inbound_payload, timestamp);
                                        break;
                                }
                                default: {
                                        BOOST_LOG_TRIVIAL(warning) << "Unhandled message type="
                                                                   << static_cast<int>(inbound_payload.header.type);
                                        break;
                                }
                        }

                        work_done++;
                }
                return work_done;
        }

        void Engine::handle_get_book_request(const core::Payload &payload)
        {
                const auto request = deserialize<core::protocol::view::GetBookRequest>(payload);

                const auto &book = get_book(request.symbol());
                auto [bids, asks] = book.snapshot();

                const auto outbound_payload = serialize(
                        payload.connectionId, core::protocol::view::GetBookResponse(std::move(bids), std::move(asks)));
                if (!f_outbound_buffer->push(*outbound_payload)) {
                        BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                }
        }

        void Engine::handle_add_order_request(core::Payload &payload, const core::Timestamp timestamp)
        {
                const auto request = deserialize<core::protocol::trading::AddOrderRequest>(payload);
                auto &book = get_book(request.symbol());

                auto [order_id, trades] =
                        book.add_order(request.price(), request.quantity(), request.side(), timestamp);

                auto outbound_payload =
                        serialize(payload.connectionId, core::protocol::trading::AddOrderResponse(order_id, timestamp));
                if (!f_outbound_buffer->push(*outbound_payload)) {
                        BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                }

                for (const auto &trade: trades) {
                        outbound_payload =
                                serialize(payload.connectionId,
                                          core::protocol::trading::Trade(trade.id(), trade.price(), trade.quantity(),
                                                                         trade.timestamp(), trade.source_order(),
                                                                         trade.matched_order()));
                        if (!f_outbound_buffer->push(*outbound_payload)) {
                                BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                        }
                }
        }
} // namespace orderbook
