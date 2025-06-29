#ifndef ORDER_H
#define ORDER_H

#include <functional>

#include "types.h"

namespace orderbook
{
        class Order
        {
        public:
                Order(const OrderId id, const Price price, const Quantity quantity, const Side side,
                      const Timestamp timestamp) :
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

                [[nodiscard]] OrderId id() const
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

                [[nodiscard]] Side side() const
                {
                        return f_side;
                }

                [[nodiscard]] Timestamp timestamp() const
                {
                        return f_timestamp;
                }

                void set_price(const Price price)
                {
                        this->f_price = price;
                }

                void set_quantity(const Quantity quantity)
                {
                        this->f_quantity = quantity;
                }

        private:
                OrderId f_id;
                Price f_price;
                Quantity f_quantity;
                Side f_side;
                Timestamp f_timestamp;
        };

        inline bool Order::operator==(const Order &order) const
        {
                return this->f_id == order.f_id;
        }

        inline bool Order::operator!=(const Order &order) const
        {
                return !(*this == order);
        }
}

template<>
struct std::hash<orderbook::Order>
{
        std::size_t operator()(const orderbook::Order &order) const noexcept
        {
                return std::hash<orderbook::OrderId>()(order.id());
        }
};

#endif //ORDER_H
