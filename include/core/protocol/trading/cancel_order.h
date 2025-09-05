#pragma once

#include <core/protocol/message.h>
#include <core/types.h>

namespace core::protocol::trading
{
        class CancelOrderRequest final : public Message
        {
        public:
                CancelOrderRequest() = default;
                CancelOrderRequest(const CancelOrderRequest &request) = default;
                CancelOrderRequest(CancelOrderRequest &&request) = default;
                CancelOrderRequest &operator=(const CancelOrderRequest &request) = default;
                CancelOrderRequest &operator=(CancelOrderRequest &&request) = default;
                ~CancelOrderRequest() override = default;

                CancelOrderRequest(Symbol symbol, OrderId order_id);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id);
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::CancelOrderRequest;
                }

                [[nodiscard]] Symbol symbol() const
                {
                        return f_symbol;
                }

                [[nodiscard]] OrderId order_id() const
                {
                        return f_order_id;
                }

        private:
                Symbol f_symbol{};
                OrderId f_order_id{};
        };

        class CancelOrderResponse final : public Message
        {
        public:
                CancelOrderResponse() = default;
                CancelOrderResponse(const CancelOrderResponse &response) = default;
                CancelOrderResponse(CancelOrderResponse &&response) = default;
                CancelOrderResponse &operator=(const CancelOrderResponse &response) = default;
                CancelOrderResponse &operator=(CancelOrderResponse &&response) = default;
                ~CancelOrderResponse() override = default;

                CancelOrderResponse(Symbol symbol, OrderId order_id, bool success);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) + sizeof(f_success);
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::CancelOrderResponse;
                }

                [[nodiscard]] Symbol symbol() const
                {
                        return f_symbol;
                }

                [[nodiscard]] OrderId order_id() const
                {
                        return f_order_id;
                }

                [[nodiscard]] bool success() const
                {
                        return f_success;
                }

        private:
                Symbol f_symbol{};
                OrderId f_order_id{};
                bool f_success{};
        };
} // namespace core::protocol::trading
