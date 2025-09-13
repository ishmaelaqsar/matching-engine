#pragma once

#include <unordered_map>
#include <vector>

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::view
{
        struct Order
        {
                Order() = default;

                Order(const OrderId &id, const Price &price, const Quantity &quantity, const Side &side,
                      const Timestamp &timestamp);

                auto operator<<(std::ostream &os) const -> std::ostream &;

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
                GetOrdersRequest(const GetOrdersRequest &) = default;
                GetOrdersRequest(GetOrdersRequest &&) = default;
                GetOrdersRequest &operator=(const GetOrdersRequest &) = default;
                GetOrdersRequest &operator=(GetOrdersRequest &&) = default;
                ~GetOrdersRequest() = default;

                explicit GetOrdersRequest(std::string username);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(unsigned const char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto username() const -> std::string;

        private:
                std::string f_username{};
        };

        class GetOrdersResponse final : public Message<GetOrdersResponse>
        {
        public:
                GetOrdersResponse() = default;
                GetOrdersResponse(const GetOrdersResponse &response) = default;
                GetOrdersResponse(GetOrdersResponse &&response) = default;
                GetOrdersResponse &operator=(const GetOrdersResponse &) = default;
                GetOrdersResponse &operator=(GetOrdersResponse &&) = default;
                ~GetOrdersResponse() = default;

                explicit GetOrdersResponse(std::unordered_map<Symbol, std::vector<Order>> orders_by_symbol);

                explicit GetOrdersResponse(std::unordered_map<Symbol, std::initializer_list<Order>> orders_by_symbol);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto orders() const -> std::unordered_map<Symbol, std::vector<Order>>;

        private:
                std::unordered_map<Symbol, std::vector<Order>> f_orders_by_symbol{};
        };
} // namespace core::protocol::view
