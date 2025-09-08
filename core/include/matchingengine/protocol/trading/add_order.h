#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
        class AddOrderRequest final : public Message<AddOrderRequest>
        {
        public:
                AddOrderRequest() = default;
                AddOrderRequest(const AddOrderRequest &request) = default;
                AddOrderRequest(AddOrderRequest &&request) = default;
                AddOrderRequest &operator=(const AddOrderRequest &request) = default;
                AddOrderRequest &operator=(AddOrderRequest &&request) = default;
                ~AddOrderRequest() = default;

                AddOrderRequest(Symbol symbol, Price price, Quantity quantity, Side side);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto to_string() const -> std::string;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto symbol() const -> Symbol;

                [[nodiscard]] auto price() const -> Price;

                [[nodiscard]] auto quantity() const -> Quantity;

                [[nodiscard]] auto side() const -> Side;

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
                AddOrderResponse(const AddOrderResponse &response) = default;
                AddOrderResponse(AddOrderResponse &&response) = default;
                AddOrderResponse &operator=(const AddOrderResponse &response) = default;
                AddOrderResponse &operator=(AddOrderResponse &&response) = default;
                ~AddOrderResponse() = default;

                AddOrderResponse(Symbol symbol, OrderId order_id, Price price, Quantity quantity, Side side,
                                 Timestamp timestamp);

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

                [[nodiscard]] auto side() const -> Side;

                [[nodiscard]] auto timestamp() const -> Timestamp;

        private:
                Symbol f_symbol{};
                OrderId f_order_id{};
                Price f_price{};
                Quantity f_quantity{};
                Side f_side = Side::Unknown;
                Timestamp f_timestamp{};
        };
} // namespace core::protocol::trading
