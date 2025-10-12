#include <matchingengine/orderbook/order.h>

namespace orderbook
{

    Order::Order(
        const core::OrderId id, const core::Price price,
        const core::Quantity quantity, const core::Side side,
        const core::Timestamp timestamp
    )
        : f_id(id), f_price(price), f_quantity(quantity), f_side(side),
          f_timestamp(timestamp)
    {
    }

    bool Order::operator==(const Order& order) const
    {
        return this->f_id == order.f_id;
    }

    bool Order::operator!=(const Order& order) const
    {
        return !(*this == order);
    }

    std::ostream& operator<<(std::ostream& os, const Order& order)
    {
        os << "Order{";
        os << "id: " << order.f_id << ", ";
        os << "price: " << order.f_price << ", ";
        os << "quantity: " << order.f_quantity << ", ";
        os << "side: " << order.f_side << ", ";
        os << "timestamp: " << order.f_timestamp;
        os << "}";
        return os;
    }

    void Order::set_quantity(
        const core::Quantity quantity, const core::Timestamp timestamp
    )
    {
        f_quantity = quantity;
        f_timestamp = timestamp;
    }

    void Order::add_quantity(const core::Quantity quantity)
    {
        f_quantity += quantity;
    }

    void Order::remove_quantity(const core::Quantity quantity)
    {
        f_quantity -= quantity;
    }

    core::OrderId Order::id() const
    {
        return f_id;
    }

    core::Price Order::price() const
    {
        return f_price;
    }

    core::Quantity Order::quantity() const
    {
        return f_quantity;
    }

    core::Side Order::side() const
    {
        return f_side;
    }

    core::Timestamp Order::timestamp() const
    {
        return f_timestamp;
    }
} // namespace orderbook
