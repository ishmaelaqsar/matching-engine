#ifndef ORDER_H
#define ORDER_H

#include <functional>

#include "../common/types.h"

namespace orderbook
{
        class Order
        {
        public:
                Order(const common::OrderId id, const common::Price price, const common::Quantity quantity,
                      const common::Side side,
                      const common::Timestamp timestamp) :
                        f_id(id),
                        f_price(price),
                        f_quantity(quantity),
                        f_side(side),
                        f_timestamp(timestamp) {}

                Order(const Order &order) = default;

                Order(Order &&order) = default;

                Order &operator=(const Order &order) = default;

                Order &operator=(Order &&order) = default;

                ~Order() = default;

                bool operator==(const Order &order) const;

                bool operator!=(const Order &order) const;

                friend std::ostream &operator<<(std::ostream &os, const Order &order);

                [[nodiscard]] common::OrderId id() const
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

                [[nodiscard]] common::Side side() const
                {
                        return f_side;
                }

                [[nodiscard]] common::Timestamp timestamp() const
                {
                        return f_timestamp;
                }

                void set_price(const common::Price price)
                {
                        this->f_price = price;
                }

                void set_quantity(const common::Quantity quantity)
                {
                        this->f_quantity = quantity;
                }

        private:
                common::OrderId f_id;
                common::Price f_price;
                common::Quantity f_quantity;
                common::Side f_side;
                common::Timestamp f_timestamp;
        };

        inline bool Order::operator==(const Order &order) const
        {
                return this->f_id == order.f_id;
        }

        inline bool Order::operator!=(const Order &order) const
        {
                return !(*this == order);
        }

        inline std::ostream &operator<<(std::ostream &os, const Order &order)
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
}

template<>
struct std::hash<orderbook::Order>
{
        std::size_t operator()(const orderbook::Order &order) const noexcept
        {
                return std::hash<common::OrderId>()(order.id());
        }
};

#endif //ORDER_H
