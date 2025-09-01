#ifndef PROTOCOL_MODIFY_ORDER_H
#define PROTOCOL_MODIFY_ORDER_H

#include <iostream>

#include "../../types.h"
#include "../serialize_helper.h"
#include "common/protocol/message.h"

namespace common::protocol::trading
{
        class ModifyOrderRequest final : Message
        {
        public:
                ModifyOrderRequest() = default;
                ModifyOrderRequest(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest(ModifyOrderRequest &&request) = default;
                ModifyOrderRequest &operator=(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest &operator=(ModifyOrderRequest &&request) = default;
                ~ModifyOrderRequest() override = default;

                ModifyOrderRequest(const OrderId order_id, const Price price, const Quantity quantity) :
                    f_order_id(order_id), f_price(price), f_quantity(quantity)
                {}

                void serialize(unsigned char *dst) const override
                {
                        size_t offset = 0;
                        serialize_uint64(f_order_id, dst, &offset);
                        serialize_uint64(f_price, dst, &offset);
                        serialize_uint64(f_quantity, dst, &offset);
                }

                void deserialize(const unsigned char *src) override
                {
                        size_t offset = 0;
                        f_order_id = deserialize_uint64(src, &offset);
                        f_price = deserialize_uint64(src, &offset);
                        f_quantity = deserialize_uint64(src, &offset);
                }

                void print(std::ostream &os) const override
                {
                        os << "ModifyOrderRequest{";
                        os << "order_id: " << f_order_id << ", ";
                        os << "price: " << f_price << ", ";
                        os << "quantity: " << f_quantity << ", ";
                        os << "}";
                }

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(f_order_id) + sizeof(f_price) + sizeof(f_quantity);
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::ModifyOrderRequest;
                }

                [[nodiscard]] OrderId order_id() const
                {
                        return f_order_id;
                }

                [[nodiscard]] Price price() const
                {
                        return f_price;
                }

                [[nodiscard]] Quantity quantity() const
                {
                        return f_quantity;
                }

        private:
                OrderId f_order_id{};
                Price f_price{};
                Quantity f_quantity{};
        };

        class ModifyOrderResponse final : public Message
        {
        public:
                ModifyOrderResponse() = default;
                ModifyOrderResponse(const ModifyOrderResponse &response) = default;
                ModifyOrderResponse(ModifyOrderResponse &&response) = default;
                ModifyOrderResponse &operator=(const ModifyOrderResponse &response) = default;
                ModifyOrderResponse &operator=(ModifyOrderResponse &&response) = default;
                ~ModifyOrderResponse() override = default;

                ModifyOrderResponse(const OrderId order_id, const Price price, const Quantity quantity,
                                    const bool success) :
                    f_order_id(order_id), f_price(price), f_quantity(quantity), f_success(success)
                {}

                void serialize(unsigned char *dst) const override
                {
                        size_t offset = 0;
                        serialize_uint64(f_order_id, dst, &offset);
                        serialize_uint64(f_price, dst, &offset);
                        serialize_uint64(f_quantity, dst, &offset);
                        serialize_uint8(f_success, dst, &offset);
                }

                void deserialize(const unsigned char *src) override
                {
                        size_t offset = 0;
                        f_order_id = deserialize_uint64(src, &offset);
                        f_price = deserialize_uint64(src, &offset);
                        f_quantity = deserialize_uint64(src, &offset);
                        f_success = static_cast<bool>(deserialize_uint8(src, &offset));
                }

                void print(std::ostream &os) const override
                {
                        os << "ModifyOrderResponse{";
                        os << "order_id: " << f_order_id << ", ";
                        os << "price: " << f_price << ", ";
                        os << "quantity: " << f_quantity << ", ";
                        os << "success: " << f_success;
                        os << "}";
                }

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(f_order_id) + sizeof(f_price) + sizeof(f_quantity) + sizeof(f_success);
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::ModifyOrderResponse;
                }

                [[nodiscard]] OrderId order_id() const
                {
                        return f_order_id;
                }

                [[nodiscard]] Price price() const
                {
                        return f_price;
                }

                [[nodiscard]] Quantity quantity() const
                {
                        return f_quantity;
                }

                [[nodiscard]] bool success() const
                {
                        return f_success;
                }

        private:
                OrderId f_order_id{};
                Price f_price{};
                Quantity f_quantity{};
                bool f_success{};
        };
} // namespace common::protocol::trading

#endif // PROTOCOL_MODIFY_ORDER_H
