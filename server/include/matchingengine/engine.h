#pragma once

#include <matchingengine/orderbook/book.h>
#include <matchingengine/ring_buffer.h>
#include <matchingengine/types.h>
#include <sqlite3.h>
#include <unordered_map>

namespace orderbook
{
    class Engine
    {
    public:
        Engine(
            sqlite3* db, core::RingBuffer<core::Payload>& inbound_buffer,
            core::RingBuffer<core::Payload>& outbound_buffer
        );

        auto do_work() -> int;

    private:
        template <typename Message>
        static core::Payload
        serialize(core::ConnectionId connectionId, Message&& response);

        template <typename SubMessage>
        static SubMessage deserialize(const core::Payload& payload);

        Book& get_book(const core::Symbol& symbol);

        void handle_get_book_request(const core::Payload& payload);
        void handle_add_order_request(
            const core::Payload& payload, core::Timestamp timestamp
        );
        void handle_modify_order_request(
            const core::Payload& payload, core::Timestamp timestamp
        );
        void handle_cancel_order_request(const core::Payload& payload);

        std::unordered_map<core::Symbol, Book> f_order_books{};
        sqlite3* f_db;
        core::RingBuffer<core::Payload>& f_inbound_buffer;
        core::RingBuffer<core::Payload>& f_outbound_buffer;
    };

    template <typename Message>
    core::Payload
    Engine::serialize(const core::ConnectionId connectionId, Message&& response)
    {
        auto payload = core::Payload{};
        payload.connection_id = connectionId;
        payload.header = {response.type(), response.size()};
        response.serialize(payload.data.data());
        return payload;
    }

    template <typename SubMessage>
    SubMessage Engine::deserialize(const core::Payload& payload)
    {
        static_assert(
            std::is_base_of_v<core::protocol::Message<SubMessage>, SubMessage>,
            "SubMessage must derive from Message<SubMessage>"
        );

        SubMessage msg = SubMessage{};
        msg.deserialize(payload.data.data());
        return msg;
    }
} // namespace orderbook
