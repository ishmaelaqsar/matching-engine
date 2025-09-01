#include "orderbook/trade.h"

namespace orderbook
{
        Trade::Trade(const core::TradeId id, const core::Price price, const core::Quantity quantity,
                     const core::Timestamp timestamp, const core::OrderId source_order,
                     const core::OrderId matched_order) :
            f_id(id),
            f_price(price),
            f_quantity(quantity),
            f_timestamp(timestamp),
            f_source_order(source_order),
            f_matched_order(matched_order)
        {}

        bool Trade::operator==(const Trade &trade) const
        {
                return f_id == trade.f_id;
        }

        bool Trade::operator!=(const Trade &trade) const
        {
                return !(*this == trade);
        }

        std::ostream &operator<<(std::ostream &os, const Trade &trade)
        {
                os << "Trade{";
                os << "id: " << trade.f_id << ", ";
                os << "price: " << trade.f_price << ", ";
                os << "quantity: " << trade.f_quantity << ", ";
                os << "timestamp: " << trade.f_timestamp << ", ";
                os << "source_order: " << trade.f_source_order << ", ";
                os << "matched_order: " << trade.f_matched_order;
                os << "}";
                return os;
        }
} // namespace orderbook
