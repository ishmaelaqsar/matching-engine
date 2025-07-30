#ifndef TRADE_H
#define TRADE_H

#include "order.h"
#include "../common/types.h"

namespace orderbook
{
        class Trade
        {
        public:
                Trade(const common::TradeId id, const common::Price price, const common::Quantity quantity, const common::Timestamp timestamp,
                      const common::OrderId source_order, const common::OrderId matched_order) :
                        f_id(id),
                        f_price(price),
                        f_quantity(quantity),
                        f_timestamp(timestamp),
                        f_source_order(source_order),
                        f_matched_order(matched_order) {}

                Trade(const Trade &trade) = default;

                Trade(Trade &&trade) = default;

                Trade &operator=(const Trade &trade) = default;

                Trade &operator=(Trade &&trade) = default;

                ~Trade() = default;

                bool operator==(const Trade &trade) const;

                bool operator!=(const Trade &trade) const;

                [[nodiscard]] common::TradeId id() const
                {
                        return f_id;
                }

                [[nodiscard]] common::Price price() const
                {
                        return f_price;
                }

                [[nodiscard]] common::Quantity quantity() const
                {
                        return f_quantity;
                }

                [[nodiscard]] common::Timestamp timestamp() const
                {
                        return f_timestamp;
                }

                [[nodiscard]] common::OrderId source_order() const
                {
                        return f_source_order;
                }

                [[nodiscard]] common::OrderId matched_order() const
                {
                        return f_matched_order;
                }

        private:
                common::TradeId f_id;
                common::Price f_price;
                common::Quantity f_quantity;
                common::Timestamp f_timestamp;
                common::OrderId f_source_order;
                common::OrderId f_matched_order;
        };

        inline bool Trade::operator==(const Trade &trade) const
        {
                return f_id == trade.f_id;
        }

        inline bool Trade::operator!=(const Trade &trade) const
        {
                return !(*this == trade);
        }
}

template<>
struct std::hash<orderbook::Trade>
{
        std::size_t operator()(const orderbook::Trade &trade) const noexcept
        {
                return std::hash<common::TradeId>()(trade.id());
        }
};

#endif //TRADE_H
