#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
    class AddOrderRequest final : public Message<AddOrderRequest>
    {
    public:
        AddOrderRequest() = default;
        AddOrderRequest(const AddOrderRequest& request) = default;
        AddOrderRequest(AddOrderRequest&& request) = default;
        AddOrderRequest& operator=(const AddOrderRequest& request) = default;
        AddOrderRequest& operator=(AddOrderRequest&& request) = default;
        ~AddOrderRequest() = default;

        AddOrderRequest(
            Symbol symbol, Price price, Quantity quantity, Side side
        );

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

        [[nodiscard]] Price price() const;

        [[nodiscard]] Quantity quantity() const;

        [[nodiscard]] Side side() const;

    private:
        Symbol f_symbol{};
        Price f_price{};
        Quantity f_quantity{};
        Side f_side = Side::Unknown;
    };

    class AddOrderResponse final : public Message<AddOrderResponse>
    {
    public:
        AddOrderResponse() = default;
        AddOrderResponse(const AddOrderResponse& response) = default;
        AddOrderResponse(AddOrderResponse&& response) = default;
        AddOrderResponse& operator=(const AddOrderResponse& response) = default;
        AddOrderResponse& operator=(AddOrderResponse&& response) = default;
        ~AddOrderResponse() = default;

        AddOrderResponse(
            Symbol symbol, OrderId order_id, Price price, Quantity quantity,
            Side side, Timestamp timestamp
        );

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

        [[nodiscard]] OrderId order_id() const;

        [[nodiscard]] Price price() const;

        [[nodiscard]] Quantity quantity() const;

        [[nodiscard]] Side side() const;

        [[nodiscard]] Timestamp timestamp() const;

    private:
        Symbol f_symbol{};
        OrderId f_order_id{};
        Price f_price{};
        Quantity f_quantity{};
        Side f_side = Side::Unknown;
        Timestamp f_timestamp{};
    };
} // namespace core::protocol::trading
