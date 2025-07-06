#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include "header.h"

namespace orderbook
{
        struct AddOrderRequest
        {
                uint64_t price;
                uint64_t quantity;
                Side side;

                static constexpr size_t SIZE = sizeof(price) + sizeof(quantity) + sizeof(side);

                static void serialize(const struct AddOrderRequest request, char *data)
                {
                        const uint64_t p = htobe64(request.price);
                        const uint64_t q = htobe64(request.quantity);
                        std::memcpy(data, &p, sizeof(p));
                        std::memcpy(data + 8, &q, sizeof(q));
                        data[16] = static_cast<uint8_t>(request.side); // NOLINT(*-narrowing-conversions)
                }

                static AddOrderRequest deserialize(const char *data)
                {
                        uint64_t p, q;
                        std::memcpy(&p, data, sizeof(p));
                        std::memcpy(&q, data + 8, sizeof(q));
                        const auto price = be64toh(p);
                        const auto quantity = be64toh(q);
                        const auto side = static_cast<Side>(data[16]);
                        return {price, quantity, side};
                }
        };

        struct AddOrderResponse
        {
                uint64_t order_id;
                uint64_t timestamp;

                static constexpr size_t SIZE = sizeof(order_id) + sizeof(timestamp);

                static void serialize(const struct AddOrderResponse response, char *data)
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
