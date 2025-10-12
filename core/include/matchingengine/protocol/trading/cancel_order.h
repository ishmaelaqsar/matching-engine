#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
    class CancelOrderRequest final : public Message<CancelOrderRequest>
    {
    public:
        CancelOrderRequest() = default;
        CancelOrderRequest(const CancelOrderRequest& request) = default;
        CancelOrderRequest(CancelOrderRequest&& request) = default;
        CancelOrderRequest& operator=(const CancelOrderRequest& request
        ) = default;
        CancelOrderRequest& operator=(CancelOrderRequest&& request) = default;
        ~CancelOrderRequest() = default;

        CancelOrderRequest(Symbol symbol, OrderId order_id);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

        [[nodiscard]] OrderId order_id() const;

    private:
        Symbol f_symbol{};
        OrderId f_order_id{};
    };

    class CancelOrderResponse final : public Message<CancelOrderResponse>
    {
    public:
        CancelOrderResponse() = default;
        CancelOrderResponse(const CancelOrderResponse& response) = default;
        CancelOrderResponse(CancelOrderResponse&& response) = default;
        CancelOrderResponse& operator=(const CancelOrderResponse& response
        ) = default;
        CancelOrderResponse& operator=(CancelOrderResponse&& response
        ) = default;
        ~CancelOrderResponse() = default;

        CancelOrderResponse(Symbol symbol, OrderId order_id, bool success);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

        [[nodiscard]] OrderId order_id() const;

        [[nodiscard]] bool success() const;

    private:
        Symbol f_symbol{};
        OrderId f_order_id{};
        bool f_success{};
    };
} // namespace core::protocol::trading
