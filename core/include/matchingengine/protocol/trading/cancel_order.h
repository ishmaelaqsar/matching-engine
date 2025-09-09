#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
        class CancelOrderRequest final : public Message<CancelOrderRequest>
        {
        public:
                CancelOrderRequest() = default;
                CancelOrderRequest(const CancelOrderRequest &request) = default;
                CancelOrderRequest(CancelOrderRequest &&request) = default;
                CancelOrderRequest &operator=(const CancelOrderRequest &request) = default;
                CancelOrderRequest &operator=(CancelOrderRequest &&request) = default;
                ~CancelOrderRequest() = default;

                CancelOrderRequest(Symbol symbol, OrderId order_id);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto symbol() const -> Symbol;

                [[nodiscard]] auto order_id() const -> OrderId;

        private:
                Symbol f_symbol{};
                OrderId f_order_id{};
        };

        class CancelOrderResponse final : public Message<CancelOrderResponse>
        {
        public:
                CancelOrderResponse() = default;
                CancelOrderResponse(const CancelOrderResponse &response) = default;
                CancelOrderResponse(CancelOrderResponse &&response) = default;
                CancelOrderResponse &operator=(const CancelOrderResponse &response) = default;
                CancelOrderResponse &operator=(CancelOrderResponse &&response) = default;
                ~CancelOrderResponse() = default;

                CancelOrderResponse(Symbol symbol, OrderId order_id, bool success);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto symbol() const -> Symbol;

                [[nodiscard]] auto order_id() const -> OrderId;

                [[nodiscard]] auto success() const -> bool;

        private:
                Symbol f_symbol{};
                OrderId f_order_id{};
                bool f_success{};
        };
} // namespace core::protocol::trading
