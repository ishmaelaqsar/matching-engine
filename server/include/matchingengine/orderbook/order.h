#pragma once

#include <functional>

#include <matchingengine/types.h>

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

                auto operator==(const Order &order) const -> bool;

                auto operator!=(const Order &order) const -> bool;

                friend auto operator<<(std::ostream &os, const Order &order) -> std::ostream &;

                auto set_price(core::Price price) -> void;

                auto set_quantity(core::Quantity quantity) -> void;

                [[nodiscard]] auto id() const -> core::OrderId;

                [[nodiscard]] auto price() const -> core::Price;

                [[nodiscard]] auto quantity() const -> core::Quantity;

                [[nodiscard]] auto side() const -> core::Side;

                [[nodiscard]] auto timestamp() const -> core::Timestamp;

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
