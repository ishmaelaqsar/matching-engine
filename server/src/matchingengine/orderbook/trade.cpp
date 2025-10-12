#include <matchingengine/orderbook/trade.h>

namespace orderbook
{
    Trade::Trade(
        const core::TradeId id, const core::Price price,
        const core::Quantity quantity, const core::Timestamp timestamp,
        const core::OrderId source_order, const core::OrderId matched_order
    )
        : f_id(id), f_price(price), f_quantity(quantity),
          f_timestamp(timestamp), f_source_order(source_order),
          f_matched_order(matched_order)
    {
    }

    bool Trade::operator==(const Trade& trade) const
    {
        return f_id == trade.f_id;
    }

    bool Trade::operator!=(const Trade& trade) const
    {
        return !(*this == trade);
    }

    std::ostream& operator<<(std::ostream& os, const Trade& trade)
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

    core::TradeId Trade::id() const
    {
        return f_id;
    }

    core::Price Trade::price() const
    {
        return f_price;
    }

    core::Quantity Trade::quantity() const
    {
        return f_quantity;
    }

    core::Timestamp Trade::timestamp() const
    {
        return f_timestamp;
    }

    core::OrderId Trade::source_order() const
    {
        return f_source_order;
    }

    core::OrderId Trade::matched_order() const
    {
        return f_matched_order;
    }
} // namespace orderbook
