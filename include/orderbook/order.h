#pragma once

#include <functional>

#include <core/types.h>

namespace orderbook
{
        class Order
        {
        public:
                Order(const Order &order) = default;
                Order(Order &&order) = default;
                Order &operator=(const Order &order) = default;
                Order &operator=(Order &&order) = default;
                ~Order() = default;

                Order(core::OrderId id, core::Price price, core::Quantity quantity, core::Side side,
                      core::Timestamp timestamp);

                bool operator==(const Order &order) const;

                bool operator!=(const Order &order) const;

                friend std::ostream &operator<<(std::ostream &os, const Order &order);

                void set_price(core::Price price);

                void set_quantity(core::Quantity quantity);

                [[nodiscard]] core::OrderId id() const
                {
                        return f_id;
                }

                [[nodiscard]] core::Price price() const
                {
                        return f_price;
                }

                [[nodiscard]] core::Quantity quantity() const
                {
                        return f_quantity;
                }

                [[nodiscard]] core::Side side() const
                {
                        return f_side;
                }

                [[nodiscard]] core::Timestamp timestamp() const
                {
                        return f_timestamp;
                }

        private:
                core::OrderId f_id;
                core::Price f_price;
                core::Quantity f_quantity;
                core::Side f_side;
                core::Timestamp f_timestamp;
        };
} // namespace orderbook

template<>
struct std::hash<orderbook::Order>
{
        std::size_t operator()(const orderbook::Order &order) const noexcept
        {
                return std::hash<core::OrderId>()(order.id());
        }
};
