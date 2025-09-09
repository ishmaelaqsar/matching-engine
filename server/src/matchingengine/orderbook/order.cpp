#include <matchingengine/orderbook/order.h>

namespace orderbook
{

        Order::Order(const core::OrderId id, const core::Price price, const core::Quantity quantity,
                     const core::Side side, const core::Timestamp timestamp) :
            f_id(id), f_price(price), f_quantity(quantity), f_side(side), f_timestamp(timestamp)
        {}

        auto Order::operator==(const Order &order) const -> bool
        {
                return this->f_id == order.f_id;
        }

        auto Order::operator!=(const Order &order) const -> bool
        {
                return !(*this == order);
        }

        auto operator<<(std::ostream &os, const Order &order) -> std::ostream &
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

        auto Order::set_quantity(const core::Quantity quantity, const core::Timestamp timestamp) -> void
        {
                f_quantity = quantity;
                f_timestamp = timestamp;
        }

        auto Order::add_quantity(const core::Quantity quantity) -> void
        {
                f_quantity += quantity;
        }

        auto Order::remove_quantity(const core::Quantity quantity) -> void
        {
                f_quantity -= quantity;
        }

        auto Order::id() const -> core::OrderId
        {
                return f_id;
        }

        auto Order::price() const -> core::Price
        {
                return f_price;
        }

        auto Order::quantity() const -> core::Quantity
        {
                return f_quantity;
        }

        auto Order::side() const -> core::Side
        {
                return f_side;
        }

        auto Order::timestamp() const -> core::Timestamp
        {
                return f_timestamp;
        }
} // namespace orderbook
