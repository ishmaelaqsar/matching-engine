#pragma once

#include <functional>
#include <matchingengine/types.h>

namespace orderbook
{
    class Order
    {
    public:
        Order(const Order& order) = delete;
        Order(Order&& order) = default;
        Order& operator=(const Order& order) = delete;
        Order& operator=(Order&& order) = default;
        ~Order() = default;

        Order(
            core::OrderId id, core::Price price, core::Quantity quantity,
            core::Side side, core::Timestamp timestamp
        );

        bool operator==(const Order& order) const;

        bool operator!=(const Order& order) const;

        friend std::ostream& operator<<(std::ostream& os, const Order& order);

        void set_quantity(core::Quantity quantity, core::Timestamp timestamp);

        void add_quantity(core::Quantity quantity);

        void remove_quantity(core::Quantity quantity);

        [[nodiscard]] core::OrderId id() const;

        [[nodiscard]] core::Price price() const;

        [[nodiscard]] core::Quantity quantity() const;

        [[nodiscard]] core::Side side() const;

        [[nodiscard]] core::Timestamp timestamp() const;

    private:
        core::OrderId f_id;
        core::Price f_price;
        core::Quantity f_quantity;
        core::Side f_side;
        core::Timestamp f_timestamp;
    };
} // namespace orderbook

template <> struct std::hash<orderbook::Order> {
    std::size_t operator()(const orderbook::Order& order) const noexcept
    {
        return std::hash<core::OrderId>()(order.id());
    }
};
