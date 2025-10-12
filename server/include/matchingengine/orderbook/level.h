#pragma once

#include <boost/log/trivial.hpp>
#include <deque>
#include <matchingengine/orderbook/order.h>
#include <matchingengine/orderbook/shared_counter.h>
#include <matchingengine/orderbook/trade.h>
#include <matchingengine/types.h>
#include <vector>

namespace orderbook
{
    class Level
    {
    public:
        using OrderCallback = std::function<void(const core::OrderId&)>;

        Level(const Level& level) = delete;
        Level(Level&& level) noexcept = default;
        ~Level() = default;
        Level& operator=(const Level& level) = delete;
        Level& operator=(Level&& level) = default;

        Level(
            core::Price price, const SharedCounter<core::TradeId>& id_counter
        );

        friend std::ostream& operator<<(std::ostream& os, const Level& level);

        [[nodiscard]] bool empty() const;

        [[nodiscard]] core::Price price() const;

        [[nodiscard]] core::Quantity quantity() const;

        void add_quantity(core::Quantity quantity);

        void remove_quantity(core::Quantity quantity);

        void add_order(std::unique_ptr<Order> order);

        void remove_order(const core::OrderId& id);

        std::vector<Trade> match_order(
            const std::unique_ptr<Order>& incoming, const OrderCallback& on_fill
        );

    private:
        core::Price f_price;
        SharedCounter<core::TradeId> f_id_counter;
        core::Quantity f_quantity = 0;

        std::deque<std::unique_ptr<Order>> f_orders;
    };

} // namespace orderbook
