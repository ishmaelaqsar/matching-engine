#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include <utility>

#include "../../types.h"
#include "../serialize_helper.h"
#include "common/protocol/message.h"

namespace common::protocol::trading
{
        class AddOrderRequest final : Message
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

                void serialize(char *data) const override
                {
                        size_t offset = 0;
                        serialize_string(f_symbol, data, &offset);
                        serialize_uint64(f_price, data, &offset);
                        serialize_uint64(f_quantity, data, &offset);
                        serialize_uint8(static_cast<uint8_t>(f_side), data, &offset);
                }

                void deserialize(const char *data) override
                {
                        size_t offset = 0;
                        f_symbol = deserialize_string(data, &offset);
                        f_price = deserialize_uint64(data, &offset);
                        f_quantity = deserialize_uint64(data, &offset);
                        f_side = static_cast<Side>(deserialize_uint8(data, &offset));
                }

                [[nodiscard]] size_t size() const override
                {
                        return f_symbol.length() + sizeof(f_price) + sizeof(f_quantity) + sizeof(f_side);
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
                Price f_price{};
                Quantity f_quantity{};
                Side f_side = Side::Unknown;
        };

        class AddOrderResponse final : Message
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

                void serialize(char *data) const override
                {
                        size_t offset = 0;
                        serialize_uint64(f_order_id, data, &offset);
                        serialize_uint64(f_timestamp, data, &offset);
                }

                void deserialize(const char *data) override
                {
                        size_t offset = 0;
                        f_order_id = deserialize_uint64(data, &offset);
                        f_timestamp = deserialize_uint64(data, &offset);
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

                friend std::ostream &operator<<(std::ostream &os, const AddOrderResponse &response)
                {
                        os << "AddOrderResponse{";
                        os << "order_id: " << response.f_order_id << ", ";
                        os << "timestamp: " << response.f_timestamp;
                        os << "}";
                        return os;
                }

        private:
                OrderId f_order_id{};
                Timestamp f_timestamp{};
        };
} // namespace common::protocol::trading

#endif // ADD_ORDER_H
