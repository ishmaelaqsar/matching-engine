#pragma once

#include <thread>
#include <unordered_map>

#include "core/ring_buffer.h"
#include "core/types.h"
#include "orderbook/book.h"

namespace orderbook
{
        class Engine
        {
        public:
                Engine(const std::shared_ptr<core::RingBuffer<core::Payload>> &inbound_buffer,
                       const std::shared_ptr<core::RingBuffer<core::Payload>> &outbound_buffer);

                int do_work();

        private:
                static std::unique_ptr<core::Payload> serialize(const core::ConnectionId connectionId,
                                                                const core::protocol::Message &response)
                {
                        auto payload = std::make_unique<core::Payload>();
                        payload->connectionId = connectionId;
                        payload->header = {response.type(), response.size()};
                        response.serialize(payload->data.data());
                        return payload;
                }

                template<typename T>
                T deserialize(const core::Payload &payload)
                {
                        T message = T{};
                        message.deserialize(payload.data.data());
                        return message;
                }

                Book &get_book(const core::Symbol &symbol)
                {
                        auto &book = f_order_books[symbol];
                        if (!book) {
                                book = std::make_unique<Book>();
                        }
                        return *book;
                }

                void handle_get_book_request(const core::Payload &payload);
                void handle_add_order_request(core::Payload &payload, core::Timestamp timestamp);

                std::unordered_map<core::Symbol, std::unique_ptr<Book>> f_order_books{};
                const std::shared_ptr<core::RingBuffer<core::Payload>> &f_inbound_buffer;
                const std::shared_ptr<core::RingBuffer<core::Payload>> &f_outbound_buffer;
        };
} // namespace orderbook
