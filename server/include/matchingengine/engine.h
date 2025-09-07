#pragma once

#include <unordered_map>

#include <matchingengine/orderbook/book.h>
#include <matchingengine/ring_buffer.h>
#include <matchingengine/types.h>

namespace orderbook
{
        class Engine
        {
        public:
                Engine(const std::shared_ptr<core::RingBuffer<core::Payload>> &inbound_buffer,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &outbound_buffer);

                auto do_work() -> int;

        private:
                template<typename Message>
                static auto serialize(core::ConnectionId connectionId, Message &&response)
                        -> std::unique_ptr<core::Payload>;

                template<typename SubMessage>
                static auto deserialize(const core::Payload &payload) -> SubMessage;

                auto get_book(const core::Symbol &symbol) -> Book &;

                auto handle_get_book_request(const core::Payload &payload) -> void;
                auto handle_add_order_request(const core::Payload &payload, core::Timestamp timestamp) -> void;
                auto handle_modify_order_request(const core::Payload &payload, core::Timestamp timestamp) -> void;
                auto handle_cancel_order_request(const core::Payload &payload) -> void;

                std::unordered_map<core::Symbol, std::unique_ptr<Book>> f_order_books{};
                const std::shared_ptr<core::RingBuffer<core::Payload>> &f_inbound_buffer;
                const std::shared_ptr<core::RingBuffer<core::Payload>> &f_outbound_buffer;
        };

        template<typename Message>
        auto Engine::serialize(const core::ConnectionId connectionId, Message &&response)
                -> std::unique_ptr<core::Payload>
        {
                auto payload = std::make_unique<core::Payload>();
                payload->connectionId = connectionId;
                payload->header = {response.type(), response.size()};
                response.serialize(payload->data.data());
                return payload;
        }

        template<typename SubMessage>
        auto Engine::deserialize(const core::Payload &payload) -> SubMessage
        {
                static_assert(std::is_base_of_v<core::protocol::Message<SubMessage>, SubMessage>,
                              "SubMessage must derive from Message<SubMessage>");

                SubMessage msg = SubMessage{};
                msg.deserialize(payload.data.data());
                return msg;
        }
} // namespace orderbook
