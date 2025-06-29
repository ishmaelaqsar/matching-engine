#ifndef TRADE_H
#define TRADE_H

#include "order.h"
#include "types.h"

namespace orderbook
{
        class Trade
        {
        public:
                Trade(const TradeId id, const Price price, const Quantity quantity, const Timestamp timestamp,
                      const OrderId source_order, const OrderId matched_order) :
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

                [[nodiscard]] TradeId id() const
                {
                        return f_id;
                }

                [[nodiscard]] Price price() const
                {
                        return f_price;
                }

                [[nodiscard]] Quantity quantity() const
                {
                        return f_quantity;
                }

                [[nodiscard]] Timestamp timestamp() const
                {
                        return f_timestamp;
                }

                [[nodiscard]] OrderId source_order() const
                {
                        return f_source_order;
                }

                [[nodiscard]] OrderId matched_order() const
                {
                        return f_matched_order;
                }

        private:
                TradeId f_id;
                Price f_price;
                Quantity f_quantity;
                Timestamp f_timestamp;
                OrderId f_source_order;
                OrderId f_matched_order;
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
                return std::hash<orderbook::TradeId>()(trade.id());
        }
};

#endif //TRADE_H
