#pragma once

#include <iostream>

#include "core/protocol/message.h"
#include "core/types.h"

namespace core::protocol::trading
{
        class ModifyOrderRequest final : public Message
        {
        public:
                ModifyOrderRequest() = default;
                ModifyOrderRequest(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest(ModifyOrderRequest &&request) = default;
                ModifyOrderRequest &operator=(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest &operator=(ModifyOrderRequest &&request) = default;
                ~ModifyOrderRequest() override = default;

                ModifyOrderRequest(OrderId order_id, Price price, Quantity quantity);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

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

                ModifyOrderResponse(OrderId order_id, Price price, Quantity quantity, bool success);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

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
} // namespace core::protocol::trading
