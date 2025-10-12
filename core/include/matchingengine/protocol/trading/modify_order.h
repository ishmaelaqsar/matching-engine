#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
    class ModifyOrderRequest final : public Message<ModifyOrderRequest>
    {
    public:
        ModifyOrderRequest() = default;
        ModifyOrderRequest(const ModifyOrderRequest& request) = default;
        ModifyOrderRequest(ModifyOrderRequest&& request) = default;
        ModifyOrderRequest& operator=(const ModifyOrderRequest& request
        ) = default;
        ModifyOrderRequest& operator=(ModifyOrderRequest&& request) = default;
        ~ModifyOrderRequest() = default;

        ModifyOrderRequest(Symbol symbol, OrderId order_id, Quantity quantity);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

        [[nodiscard]] OrderId order_id() const;

        [[nodiscard]] Quantity quantity() const;

    private:
        Symbol f_symbol{};
        OrderId f_order_id{};
        Quantity f_quantity{};
    };

    class ModifyOrderResponse final : public Message<ModifyOrderResponse>
    {
    public:
        ModifyOrderResponse() = default;
        ModifyOrderResponse(const ModifyOrderResponse& response) = default;
        ModifyOrderResponse(ModifyOrderResponse&& response) = default;
        ModifyOrderResponse& operator=(const ModifyOrderResponse& response
        ) = default;
        ModifyOrderResponse& operator=(ModifyOrderResponse&& response
        ) = default;
        ~ModifyOrderResponse() = default;

        ModifyOrderResponse(
            Symbol symbol, OrderId order_id, Quantity quantity, bool success
        );

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

        [[nodiscard]] OrderId order_id() const;

        [[nodiscard]] Quantity quantity() const;

        [[nodiscard]] bool success() const;

    private:
        Symbol f_symbol{};
        OrderId f_order_id{};
        Quantity f_quantity{};
        bool f_success{};
    };
} // namespace core::protocol::trading
