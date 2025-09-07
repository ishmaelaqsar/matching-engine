#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
        class Trade final : public Message<Trade>
        {
        public:
                Trade() = default;
                Trade(const Trade &trade) = default;
                Trade(Trade &&trade) = default;
                Trade &operator=(const Trade &trade) = default;
                Trade &operator=(Trade &&trade) = default;
                ~Trade() = default;

                Trade(Symbol symbol, TradeId id, Price price, Quantity quantity, Timestamp timestamp,
                      OrderId source_order, OrderId matched_order);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto to_string() const -> std::string;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto symbol() const -> const Symbol &;

                [[nodiscard]] auto id() const -> TradeId;

                [[nodiscard]] auto price() const -> Price;

                [[nodiscard]] auto quantity() const -> Quantity;

                [[nodiscard]] auto timestamp() const -> Timestamp;

                [[nodiscard]] auto source_order() const -> OrderId;

                [[nodiscard]] auto matched_order() const -> OrderId;

        private:
                Symbol f_symbol;
                TradeId f_id{};
                Price f_price{};
                Quantity f_quantity{};
                Timestamp f_timestamp{};
                OrderId f_source_order{};
                OrderId f_matched_order{};
        };
} // namespace core::protocol::trading
