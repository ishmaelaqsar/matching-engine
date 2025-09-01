#ifndef PROTOCOL_ADD_ORDER_H
#define PROTOCOL_ADD_ORDER_H

#include <utility>

#include "../../types.h"
#include "../serialize_helper.h"
#include "common/protocol/message.h"

namespace common::protocol::trading
{
        class AddOrderRequest final : public Message
        {
        public:
                AddOrderRequest() = default;
                AddOrderRequest(const AddOrderRequest &request) = default;
                AddOrderRequest(AddOrderRequest &&request) = default;
                AddOrderRequest &operator=(const AddOrderRequest &request) = default;
                AddOrderRequest &operator=(AddOrderRequest &&request) = default;
                ~AddOrderRequest() override = default;

                AddOrderRequest(Symbol symbol, const Price price, const Quantity quantity, const Side side) :
                    f_symbol(std::move(symbol)), f_price(price), f_quantity(quantity), f_side(side)
                {}

                void serialize(unsigned char *dst) const override
                {
                        size_t offset = 0;
                        serialize_string(f_symbol, dst, &offset);
                        serialize_uint64(f_price, dst, &offset);
                        serialize_uint64(f_quantity, dst, &offset);
                        serialize_uint8(static_cast<uint8_t>(f_side), dst, &offset);
                }

                void deserialize(const unsigned char *src) override
                {
                        size_t offset = 0;
                        f_symbol = deserialize_string(src, &offset);
                        f_price = deserialize_uint64(src, &offset);
                        f_quantity = deserialize_uint64(src, &offset);
                        f_side = static_cast<Side>(deserialize_uint8(src, &offset));
                }

                void print(std::ostream &os) const override
                {
                        os << "AddOrderRequest{";
                        os << "symbol: " << f_symbol << ", ";
                        os << "price: " << f_price << ", ";
                        os << "quantity: " << f_quantity << ", ";
                        os << "side: " << f_side;
                        os << "}";
                }

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(StrLen) + f_symbol.length() + sizeof(f_price) + sizeof(f_quantity) + sizeof(f_side);
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::AddOrderRequest;
                }

                [[nodiscard]] Symbol symbol() const
                {
                        return f_symbol;
                }

                [[nodiscard]] Price price() const
                {
                        return f_price;
                }

                [[nodiscard]] Quantity quantity() const
                {
                        return f_quantity;
                }

                [[nodiscard]] Side side() const
                {
                        return f_side;
                }

        private:
                Symbol f_symbol{};
                Price f_price{};
                Quantity f_quantity{};
                Side f_side = Side::Unknown;
        };

        class AddOrderResponse final : public Message
        {
        public:
                AddOrderResponse() = default;
                AddOrderResponse(const AddOrderResponse &response) = default;
                AddOrderResponse(AddOrderResponse &&response) = default;
                AddOrderResponse &operator=(const AddOrderResponse &response) = default;
                AddOrderResponse &operator=(AddOrderResponse &&response) = default;
                ~AddOrderResponse() override = default;

                AddOrderResponse(const OrderId order_id, const Timestamp timestamp) :
                    f_order_id(order_id), f_timestamp(timestamp)
                {}

                void serialize(unsigned char *dst) const override
                {
                        size_t offset = 0;
                        serialize_uint64(f_order_id, dst, &offset);
                        serialize_uint64(f_timestamp, dst, &offset);
                }

                void deserialize(const unsigned char *src) override
                {
                        size_t offset = 0;
                        f_order_id = deserialize_uint64(src, &offset);
                        f_timestamp = deserialize_uint64(src, &offset);
                }

                void print(std::ostream &os) const override
                {
                        os << "AddOrderResponse{";
                        os << "order_id: " << f_order_id << ", ";
                        os << "timestamp: " << f_timestamp;
                        os << "}";
                }

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(f_order_id) + sizeof(f_timestamp);
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::AddOrderResponse;
                }

                [[nodiscard]] OrderId order_id() const
                {
                        return f_order_id;
                }

                [[nodiscard]] Timestamp timestamp() const
                {
                        return f_timestamp;
                }

        private:
                OrderId f_order_id{};
                Timestamp f_timestamp{};
        };
} // namespace common::protocol::trading

#endif // PROTOCOL_ADD_ORDER_H
