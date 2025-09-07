#pragma once

#include <matchingengine/types.h>

namespace orderbook
{
        class Trade
        {
        public:
                Trade(const Trade &trade) = default;
                Trade(Trade &&trade) = default;
                Trade &operator=(const Trade &trade) = default;
                Trade &operator=(Trade &&trade) = default;
                ~Trade() = default;

                Trade(core::TradeId id, core::Price price, core::Quantity quantity, core::Timestamp timestamp,
                      core::OrderId source_order, core::OrderId matched_order);

                auto operator==(const Trade &trade) const -> bool;

                auto operator!=(const Trade &trade) const -> bool;

                friend auto operator<<(std::ostream &os, const Trade &trade) -> std::ostream &;

                [[nodiscard]] auto id() const -> core::TradeId;

                [[nodiscard]] auto price() const -> core::Price;

                [[nodiscard]] auto quantity() const -> core::Quantity;

                [[nodiscard]] auto timestamp() const -> core::Timestamp;

                [[nodiscard]] auto source_order() const -> core::OrderId;

                [[nodiscard]] auto matched_order() const -> core::OrderId;

        private:
                core::TradeId f_id;
                core::Price f_price;
                core::Quantity f_quantity;
                core::Timestamp f_timestamp;
                core::OrderId f_source_order;
                core::OrderId f_matched_order;
        };
} // namespace orderbook

template<>
struct std::hash<orderbook::Trade>
{
        std::size_t operator()(const orderbook::Trade &trade) const noexcept
        {
                return std::hash<core::TradeId>()(trade.id());
        }
};
