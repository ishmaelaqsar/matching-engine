#pragma once

#include <boost/log/trivial.hpp>
#include <vector>

#include <matchingengine/orderbook/order.h>
#include <matchingengine/orderbook/shared_counter.h>
#include <matchingengine/orderbook/trade.h>
#include <matchingengine/types.h>

namespace orderbook
{
        class Level
        {
        public:
                using OrderCallback = std::function<void(const core::OrderId &)>;

                Level(const Level &level) = delete;
                Level(Level &&level) = default;
                ~Level() = default;
                Level &operator=(const Level &level) = delete;
                Level &operator=(Level &&level) = default;

                Level(core::Price price, const SharedCounter<core::TradeId> &id_counter);

                friend auto operator<<(std::ostream &os, const Level &level) -> std::ostream &;

                [[nodiscard]] auto empty() const -> bool;

                [[nodiscard]] auto quantity() const -> core::Quantity;

                auto set_quantity(core::Quantity quantity) -> void;

                auto add_order(const std::shared_ptr<Order> &order) -> void;

                auto remove_order(const std::shared_ptr<Order> &order) -> void;

                auto match_order(const std::shared_ptr<Order> &order, const OrderCallback &order_fill_callback)
                        -> std::vector<Trade>;

        private:
                core::Price f_price;
                SharedCounter<core::TradeId> f_id_counter;
                core::Quantity f_quantity = 0;
                std::vector<std::shared_ptr<Order>> f_orders;
        };
} // namespace orderbook
