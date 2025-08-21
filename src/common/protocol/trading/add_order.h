#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include <cstring>
#include <iostream>
#include <utility>

#include "../../types.h"

namespace common::protocol::trading
{
        class AddOrderRequest
        {
        public:
                static void serialize(const AddOrderRequest &request, char *data)
                {
                        size_t offset = 0;

                        const uint8_t s = request.symbol().length();
                        std::memcpy(data + offset, &s, sizeof(s));
                        offset += sizeof(s);

                        std::memcpy(data + offset, request.symbol().data(), s);
                        offset += s;

                        const uint64_t p = htobe64(request.price());
                        std::memcpy(data + offset, &p, sizeof(p));
                        offset += sizeof(p);

                        const uint64_t q = htobe64(request.quantity());
                        std::memcpy(data + offset, &q, sizeof(q));
                        offset += sizeof(q);

                        data[offset] = static_cast<uint8_t>(request.side()); // NOLINT(*-narrowing-conversions)
                }

                static AddOrderRequest deserialize(const char *data)
                {
                        size_t offset = 0;

                        uint8_t s;
                        std::memcpy(&s, data + offset, sizeof(s));
                        offset += sizeof(s);

                        Symbol symbol;
                        symbol.resize(s);
                        std::memcpy(symbol.data(), data + offset, s);
                        offset += s;

                        uint64_t p;
                        std::memcpy(&p, data + offset, sizeof(p));
                        offset += sizeof(p);
                        const auto price = be64toh(p);

                        uint64_t q;
                        std::memcpy(&q, data + offset, sizeof(q));
                        offset += sizeof(q);
                        const auto quantity = be64toh(q);

                        const auto side = static_cast<Side>(data[offset]);
                        return {symbol, price, quantity, side};
                }

                AddOrderRequest() = default;
                AddOrderRequest(const AddOrderRequest &request) = default;
                AddOrderRequest(AddOrderRequest &&request) = default;
                AddOrderRequest &operator=(const AddOrderRequest &request) = default;
                AddOrderRequest &operator=(AddOrderRequest &&request) = default;
                ~AddOrderRequest() = default;

                AddOrderRequest(Symbol symbol, const uint64_t price, const uint64_t quantity, const Side side) :
                    f_symbol(std::move(symbol)), f_price(price), f_quantity(quantity), f_side(side)
                {}

                [[nodiscard]] uint16_t size() const
                {
                        return f_symbol.length() + sizeof(f_price) + sizeof(f_quantity) + sizeof(f_side);
                }

                [[nodiscard]] Symbol symbol() const
                {
                        return f_symbol;
                }

                [[nodiscard]] uint64_t price() const
                {
                        return f_price;
                }

                [[nodiscard]] uint64_t quantity() const
                {
                        return f_quantity;
                }

                [[nodiscard]] Side side() const
                {
                        return f_side;
                }

                friend std::ostream &operator<<(std::ostream &os, const AddOrderRequest &request)
                {
                        os << "AddOrderRequest{";
                        os << "symbol: " << request.f_symbol << ", ";
                        os << "price: " << request.f_price << ", ";
                        os << "quantity: " << request.f_quantity << ", ";
                        os << "side: " << request.f_side;
                        os << "}";
                        return os;
                }

        private:
                Symbol f_symbol{};
                uint64_t f_price{};
                uint64_t f_quantity{};
                Side f_side = Side::Unknown;
        };

        class AddOrderResponse
        {
        public:
                static void serialize(const AddOrderResponse response, char *data)
                {
                        const uint64_t id = htobe64(response.order_id());
                        const uint64_t ts = htobe64(response.timestamp());
                        std::memcpy(data, &id, sizeof(id));
                        std::memcpy(data + sizeof(id), &ts, sizeof(ts));
                }

                static AddOrderResponse deserialize(const char *data)
                {
                        uint64_t id, ts;
                        std::memcpy(&id, data, sizeof(id));
                        std::memcpy(&ts, data + sizeof(id), sizeof(ts));
                        const auto order_id = be64toh(id);
                        const auto timestamp = be64toh(ts);
                        return {order_id, timestamp};
                }

                AddOrderResponse() = default;
                AddOrderResponse(const AddOrderResponse &response) = default;
                AddOrderResponse(AddOrderResponse &&response) = default;
                AddOrderResponse &operator=(const AddOrderResponse &response) = default;
                AddOrderResponse &operator=(AddOrderResponse &&response) = default;
                ~AddOrderResponse() = default;

                AddOrderResponse(const uint64_t order_id, const uint64_t timestamp) :
                    f_order_id(order_id), f_timestamp(timestamp)
                {}

                [[nodiscard]] uint16_t size() const
                {
                        return sizeof(f_order_id) + sizeof(f_timestamp);
                }

                [[nodiscard]] uint64_t order_id() const
                {
                        return f_order_id;
                }

                [[nodiscard]] uint64_t timestamp() const
                {
                        return f_timestamp;
                }

                friend std::ostream &operator<<(std::ostream &os, const AddOrderResponse &response)
                {
                        os << "AddOrderResponse{";
                        os << "order_id: " << response.f_order_id << ", ";
                        os << "timestamp: " << response.f_timestamp;
                        os << "}";
                        return os;
                }

        private:
                uint64_t f_order_id{};
                uint64_t f_timestamp{};
        };
} // namespace common::protocol::trading

#endif // ADD_ORDER_H
