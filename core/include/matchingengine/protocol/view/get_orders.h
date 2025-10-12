#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>
#include <unordered_map>
#include <vector>

namespace core::protocol::view
{
    struct Order {
        Order() = default;

        Order(
            const OrderId& id, const Price& price, const Quantity& quantity,
            const Side& side, const Timestamp& timestamp
        );

        std::ostream& operator<<(std::ostream& os) const;

        OrderId id{};
        Price price{};
        Quantity quantity{};
        Side side{};
        Timestamp timestamp{};
    };

    class GetOrdersRequest final : public Message<GetOrdersRequest>
    {
    public:
        GetOrdersRequest() = default;
        GetOrdersRequest(const GetOrdersRequest&) = default;
        GetOrdersRequest(GetOrdersRequest&&) = default;
        GetOrdersRequest& operator=(const GetOrdersRequest&) = default;
        GetOrdersRequest& operator=(GetOrdersRequest&&) = default;
        ~GetOrdersRequest() = default;

        explicit GetOrdersRequest(std::string username);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] std::string username() const;

    private:
        std::string f_username{};
    };

    class GetOrdersResponse final : public Message<GetOrdersResponse>
    {
    public:
        GetOrdersResponse() = default;
        GetOrdersResponse(const GetOrdersResponse& response) = default;
        GetOrdersResponse(GetOrdersResponse&& response) = default;
        GetOrdersResponse& operator=(const GetOrdersResponse&) = default;
        GetOrdersResponse& operator=(GetOrdersResponse&&) = default;
        ~GetOrdersResponse() = default;

        explicit GetOrdersResponse(
            std::unordered_map<Symbol, std::vector<Order>> orders_by_symbol
        );

        explicit GetOrdersResponse(
            std::unordered_map<Symbol, std::initializer_list<Order>>
                orders_by_symbol
        );

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] std::unordered_map<Symbol, std::vector<Order>>
        orders() const;

    private:
        std::unordered_map<Symbol, std::vector<Order>> f_orders_by_symbol{};
    };
} // namespace core::protocol::view
