#ifndef MODIFY_ORDER_H
#define MODIFY_ORDER_H

#include <cstdint>
#include <cstring>
#include <endian.h>
#include <iostream>

#include "../../types.h"

namespace common::protocol::trading
{
        class ModifyOrderRequest
        {
        public:
                static void serialize(const ModifyOrderRequest &request, char *data)
                {
                        size_t offset = 0;

                        const uint64_t o = htobe64(request.f_order_id);
                        std::memcpy(data + offset, &o, sizeof(o));
                        offset += sizeof(o);

                        const uint64_t p = htobe64(request.f_price);
                        std::memcpy(data + offset, &p, sizeof(p));
                        offset += sizeof(p);

                        const uint64_t q = htobe64(request.f_quantity);
                        std::memcpy(data + offset, &q, sizeof(q));
                }

                static ModifyOrderRequest deserialize(const char *data)
                {
                        size_t offset = 0;

                        uint64_t o;
                        std::memcpy(&o, data + offset, sizeof(o));
                        offset += sizeof(o);
                        const auto order_id = be64toh(o);

                        uint64_t p;
                        std::memcpy(&p, data + offset, sizeof(p));
                        offset += sizeof(p);
                        const auto price = be64toh(p);

                        uint64_t q;
                        std::memcpy(&q, data + offset, sizeof(q));
                        const auto quantity = be64toh(q);

                        return {order_id, price, quantity};
                }

                ModifyOrderRequest() = default;
                ModifyOrderRequest(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest(ModifyOrderRequest &&request) = default;
                ModifyOrderRequest &operator=(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest &operator=(ModifyOrderRequest &&request) = default;
                ~ModifyOrderRequest() = default;

                ModifyOrderRequest(const uint64_t order_id, const uint64_t price, const uint64_t quantity) :
                    f_order_id(order_id), f_price(price), f_quantity(quantity)
                {}

                [[nodiscard]] uint16_t size() const
                {
                        return sizeof(f_order_id) + sizeof(f_price) + sizeof(f_quantity);
                }

                [[nodiscard]] uint64_t order_id() const
                {
                        return f_order_id;
                }

                [[nodiscard]] uint64_t price() const
                {
                        return f_price;
                }

                [[nodiscard]] uint64_t quantity() const
                {
                        return f_quantity;
                }

                friend std::ostream &operator<<(std::ostream &os, const ModifyOrderRequest &request)
                {
                        os << "ModifyOrderRequest{";
                        os << "order_id: " << request.f_order_id << ", ";
                        os << "price: " << request.f_price << ", ";
                        os << "quantity: " << request.f_quantity << ", ";
                        os << "}";
                        return os;
                }

        private:
                OrderId f_order_id{};
                uint64_t f_price{};
                uint64_t f_quantity{};
        };
} // namespace common::protocol::trading

#endif // MODIFY_ORDER_H
