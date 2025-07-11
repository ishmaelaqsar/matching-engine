#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include "header.h"

namespace orderbook
{
        constexpr StrLen MAX_SYMBOL_LEN = 8;

        struct AddOrderRequest
        {
                std::string symbol;
                uint64_t price;
                uint64_t quantity;
                Side side;

                static constexpr size_t SIZE = MAX_SYMBOL_LEN + sizeof(price) + sizeof(quantity) + sizeof(side);

                static void serialize(const AddOrderRequest &request, char *data)
                {
                        auto offset = serialize_string(request.symbol, data, 0, MAX_SYMBOL_LEN);

                        const uint64_t p = htobe64(request.price);
                        const uint64_t q = htobe64(request.quantity);
                        std::memcpy(data + offset, &p, sizeof(p));
                        offset += sizeof(p);

                        std::memcpy(data + offset, &q, sizeof(q));
                        offset += sizeof(q);

                        data[offset] = static_cast<uint8_t>(request.side); // NOLINT(*-narrowing-conversions)
                }

                static AddOrderRequest deserialize(const char *data)
                {
                        size_t offset = 0;
                        auto symbol = deserialize_string(data, &offset, MAX_SYMBOL_LEN);

                        uint64_t p;
                        std::memcpy(&p, data + offset, sizeof(p));
                        offset += sizeof(p);
                        const auto price = be64toh(p);

                        uint64_t q;
                        std::memcpy(&q, data + offset, sizeof(q));
                        offset += sizeof(q);
                        const auto quantity = be64toh(q);

                        const auto side = static_cast<Side>(data[offset]);
                        return {std::move(symbol), price, quantity, side};
                }
        };

        struct AddOrderResponse
        {
                uint64_t order_id;
                uint64_t timestamp;

                static constexpr size_t SIZE = sizeof(order_id) + sizeof(timestamp);

                static void serialize(const AddOrderResponse response, char *data)
                {
                        const uint64_t id = htobe64(response.order_id);
                        const uint64_t ts = htobe64(response.timestamp);
                        std::memcpy(data, &id, sizeof(id));
                        std::memcpy(data + 8, &ts, sizeof(ts));
                }

                static AddOrderResponse deserialize(const char *data)
                {
                        uint64_t id, ts;
                        std::memcpy(&id, data, sizeof(id));
                        std::memcpy(&ts, data + 8, sizeof(ts));
                        const auto order_id = be64toh(id);
                        const auto timestamp = be64toh(ts);
                        return {order_id, timestamp};
                }
        };
}

#endif //ADD_ORDER_H
