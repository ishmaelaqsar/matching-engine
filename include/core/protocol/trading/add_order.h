#pragma once

#include <utility>

#include "core/types.h"
#include "core/protocol/message.h"

namespace core::protocol::trading
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

                AddOrderRequest(Symbol symbol, Price price, Quantity quantity, Side side);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

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

                AddOrderResponse(OrderId order_id, Timestamp timestamp);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

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
} // namespace core::protocol::trading