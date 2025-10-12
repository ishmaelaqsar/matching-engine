#pragma once

#include <matchingengine/types.h>

namespace orderbook
{
    class Trade
    {
    public:
        Trade(const Trade& trade) = default;
        Trade(Trade&& trade) = default;
        Trade& operator=(const Trade& trade) = default;
        Trade& operator=(Trade&& trade) = default;
        ~Trade() = default;

        Trade(
            core::TradeId id, core::Price price, core::Quantity quantity,
            core::Timestamp timestamp, core::OrderId source_order,
            core::OrderId matched_order
        );

        bool operator==(const Trade& trade) const;

        bool operator!=(const Trade& trade) const;

        friend std::ostream& operator<<(std::ostream& os, const Trade& trade);

        [[nodiscard]] core::TradeId id() const;

        [[nodiscard]] core::Price price() const;

        [[nodiscard]] core::Quantity quantity() const;

        [[nodiscard]] core::Timestamp timestamp() const;

        [[nodiscard]] core::OrderId source_order() const;

        [[nodiscard]] core::OrderId matched_order() const;

    private:
        core::TradeId f_id;
        core::Price f_price;
        core::Quantity f_quantity;
        core::Timestamp f_timestamp;
        core::OrderId f_source_order;
        core::OrderId f_matched_order;
    };
} // namespace orderbook

template <> struct std::hash<orderbook::Trade> {
    std::size_t operator()(const orderbook::Trade& trade) const noexcept
    {
        return std::hash<core::TradeId>()(trade.id());
    }
};
