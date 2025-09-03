#include <orderbook/order.h>

namespace orderbook
{

        Order::Order(const core::OrderId id, const core::Price price, const core::Quantity quantity,
                     const core::Side side, const core::Timestamp timestamp) :
            f_id(id), f_price(price), f_quantity(quantity), f_side(side), f_timestamp(timestamp)
        {}

        bool Order::operator==(const Order &order) const
        {
                return this->f_id == order.f_id;
        }

        bool Order::operator!=(const Order &order) const
        {
                return !(*this == order);
        }

        void Order::set_price(const core::Price price)
        {
                this->f_price = price;
        }

        void Order::set_quantity(const core::Quantity quantity)
        {
                this->f_quantity = quantity;
        }

        std::ostream &operator<<(std::ostream &os, const Order &order)
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
} // namespace orderbook
