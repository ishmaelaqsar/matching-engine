#include <matchingengine/engine.h>

#include <boost/log/trivial.hpp>
#include <chrono>

#include <matchingengine/protocol/message.h>
#include <matchingengine/protocol/trading/add_order.h>
#include <matchingengine/protocol/trading/trade.h>
#include <matchingengine/protocol/view/get_book.h>
#include "matchingengine/protocol/trading/cancel_order.h"
#include "matchingengine/protocol/trading/modify_order.h"

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

                        const auto timestamp =
                                static_cast<core::Timestamp>(std::chrono::time_point_cast<std::chrono::milliseconds>(
                                                                     std::chrono::system_clock::now())
                                                                     .time_since_epoch()
                                                                     .count());

                        switch (inbound_payload.header.type) {
                                case core::protocol::MessageType::GetBookRequest: {
                                        handle_get_book_request(inbound_payload);
                                        break;
                                }
                                case core::protocol::MessageType::AddOrderRequest: {
                                        handle_add_order_request(inbound_payload, timestamp);
                                        break;
                                }
                                case core::protocol::MessageType::ModifyOrderRequest: {
                                        handle_modify_order_request(inbound_payload, timestamp);
                                        break;
                                }
                                case core::protocol::MessageType::CancelOrderRequest: {
                                        handle_cancel_order_request(inbound_payload);
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

        Book &Engine::get_book(const core::Symbol &symbol)
        {
                auto &book = f_order_books[symbol];
                if (!book) {
                        book = std::make_unique<Book>();
                }
                return *book;
        }

        void Engine::handle_get_book_request(const core::Payload &payload)
        {
                const auto request = deserialize<core::protocol::view::GetBookRequest>(payload);

                BOOST_LOG_TRIVIAL(debug) << "Connection ID=" << payload.connectionId << ", " << request;

                const auto &book = get_book(request.symbol());
                auto [bids, asks] = book.snapshot();

                const auto outbound_payload = serialize(
                        payload.connectionId, core::protocol::view::GetBookResponse(request.symbol(), bids, asks));
                if (!f_outbound_buffer->push(*outbound_payload)) {
                        BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                }
        }

        void Engine::handle_add_order_request(const core::Payload &payload, const core::Timestamp timestamp)
        {
                const auto request = deserialize<core::protocol::trading::AddOrderRequest>(payload);

                BOOST_LOG_TRIVIAL(debug) << "Connection ID=" << payload.connectionId << ", " << request;

                auto &book = get_book(request.symbol());

                auto [order_id, trades] =
                        book.add_order(request.price(), request.quantity(), request.side(), timestamp);

                auto outbound_payload = serialize(
                        payload.connectionId,
                        core::protocol::trading::AddOrderResponse(request.symbol(), order_id, request.price(),
                                                                  request.quantity(), request.side(), timestamp));
                if (!f_outbound_buffer->push(*outbound_payload)) {
                        BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                }

                for (const auto &trade: trades) {
                        outbound_payload =
                                serialize(payload.connectionId,
                                          core::protocol::trading::Trade(request.symbol(), trade.id(), trade.price(),
                                                                         trade.quantity(), trade.timestamp(),
                                                                         trade.source_order(), trade.matched_order()));
                        if (!f_outbound_buffer->push(*outbound_payload)) {
                                BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                        }
                }
        }

        void Engine::handle_modify_order_request(const core::Payload &payload, const core::Timestamp timestamp)
        {
                const auto request = deserialize<core::protocol::trading::ModifyOrderRequest>(payload);

                BOOST_LOG_TRIVIAL(debug) << "Connection ID=" << payload.connectionId << ", " << request;

                auto &book = get_book(request.symbol());

                auto [success, trades] =
                        book.modify_order(request.order_id(), request.price(), request.quantity(), timestamp);

                auto outbound_payload = serialize(
                        payload.connectionId,
                        core::protocol::trading::ModifyOrderResponse(request.symbol(), request.order_id(), success));
                if (!f_outbound_buffer->push(*outbound_payload)) {
                        BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                }

                for (const auto &trade: trades) {
                        outbound_payload =
                                serialize(payload.connectionId,
                                          core::protocol::trading::Trade(request.symbol(), trade.id(), trade.price(),
                                                                         trade.quantity(), trade.timestamp(),
                                                                         trade.source_order(), trade.matched_order()));
                        if (!f_outbound_buffer->push(*outbound_payload)) {
                                BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                        }
                }
        }

        void Engine::handle_cancel_order_request(const core::Payload &payload)
        {
                const auto request = deserialize<core::protocol::trading::CancelOrderRequest>(payload);

                BOOST_LOG_TRIVIAL(debug) << "Connection ID=" << payload.connectionId << ", " << request;

                auto &book = get_book(request.symbol());

                const auto success = book.remove_order(request.order_id());

                const auto outbound_payload = serialize(
                        payload.connectionId,
                        core::protocol::trading::CancelOrderResponse(request.symbol(), request.order_id(), success));
                if (!f_outbound_buffer->push(*outbound_payload)) {
                        BOOST_LOG_TRIVIAL(warning) << "Outbound buffer is full, dropping message";
                }
        }
} // namespace orderbook
