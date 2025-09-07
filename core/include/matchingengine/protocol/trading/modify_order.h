#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
        class ModifyOrderRequest final : public Message<ModifyOrderRequest>
        {
        public:
                ModifyOrderRequest() = default;
                ModifyOrderRequest(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest(ModifyOrderRequest &&request) = default;
                ModifyOrderRequest &operator=(const ModifyOrderRequest &request) = default;
                ModifyOrderRequest &operator=(ModifyOrderRequest &&request) = default;
                ~ModifyOrderRequest() = default;

                ModifyOrderRequest(Symbol symbol, OrderId order_id, Price price, Quantity quantity);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto to_string() const -> std::string;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto symbol() const -> Symbol;

                [[nodiscard]] auto order_id() const -> OrderId;

                [[nodiscard]] auto price() const -> Price;

                [[nodiscard]] auto quantity() const -> Quantity;

        private:
                Symbol f_symbol{};
                OrderId f_order_id{};
                Price f_price{};
                Quantity f_quantity{};
        };

        class ModifyOrderResponse final : public Message<ModifyOrderResponse>
        {
        public:
                ModifyOrderResponse() = default;
                ModifyOrderResponse(const ModifyOrderResponse &response) = default;
                ModifyOrderResponse(ModifyOrderResponse &&response) = default;
                ModifyOrderResponse &operator=(const ModifyOrderResponse &response) = default;
                ModifyOrderResponse &operator=(ModifyOrderResponse &&response) = default;
                ~ModifyOrderResponse() = default;

                ModifyOrderResponse(Symbol symbol, OrderId order_id, bool success);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto to_string() const -> std::string;

                auto print(std::ostream &os) const -> void;

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
