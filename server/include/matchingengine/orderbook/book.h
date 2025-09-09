#pragma once

#include <map>
#include <unordered_map>
#include <vector>

#include <matchingengine/orderbook/level.h>
#include <matchingengine/orderbook/order.h>
#include <matchingengine/orderbook/shared_counter.h>
#include <matchingengine/orderbook/trade.h>
#include <matchingengine/types.h>

namespace orderbook
{
        struct Snapshot
        {
                std::vector<std::pair<core::Price, core::Quantity>> bids;
                std::vector<std::pair<core::Price, core::Quantity>> asks;
        };

        class Book
        {
        public:
                using LevelRemoval = std::function<void()>;

                Book() = default;
                Book(const Book &book) = default;
                Book(Book &&book) = default;
                Book &operator=(const Book &book) = default;
                Book &operator=(Book &&book) = default;
                ~Book() = default;

                auto add_order(const core::Price &price, const core::Quantity &quantity, const core::Side &side,
                               const core::Timestamp &timestamp) -> std::pair<core::OrderId, std::vector<Trade>>;

                auto modify_order(const core::OrderId &order_id, const core::Quantity &quantity, const core::Timestamp &timestamp) -> bool;

                auto remove_order(const core::OrderId &order_id) -> bool;

                auto snapshot() const -> Snapshot;

        private:
                static auto buy_check(core::Price level_price, core::Price order_price) -> bool;

                static auto sell_check(core::Price level_price, core::Price order_price) -> bool;

                auto get_level(const Order &order) -> std::pair<Level *, LevelRemoval>;

                auto add_order(const core::OrderId &order_id, const core::Price &price, const core::Quantity &quantity,
                               const core::Side &side, const core::Timestamp &timestamp) -> std::vector<Trade>;

                template<typename Compare1, typename Compare2, typename PriceCheck>
                auto match(std::unique_ptr<Order> order,
                           std::map<core::Price, std::unique_ptr<Level>, Compare1> &levels,
                           std::map<core::Price, std::unique_ptr<Level>, Compare2> &opposite_levels,
                           PriceCheck &price_check) -> std::vector<Trade>;

                std::unordered_map<core::OrderId, Order *> f_orders{};
                std::map<core::Price, std::unique_ptr<Level>, std::greater<>> f_bids{};
                std::map<core::Price, std::unique_ptr<Level>, std::less<>> f_asks{};

                SharedCounter<core::OrderId> f_order_counter = SharedCounter<core::OrderId>();
                SharedCounter<core::TradeId> f_trade_counter = SharedCounter<core::TradeId>();
        };

        template<typename Compare1, typename Compare2, typename PriceCheck>
        auto Book::match(std::unique_ptr<Order> order, std::map<core::Price, std::unique_ptr<Level>, Compare1> &levels,
                         std::map<core::Price, std::unique_ptr<Level>, Compare2> &opposite_levels,
                         PriceCheck &price_check) -> std::vector<Trade>
        {
                std::vector<Trade> trades{};

                for (auto it = opposite_levels.begin(); it != opposite_levels.end() && order->quantity() > 0;) {
                        if (!price_check(it->first, order->price())) {
                                break;
                        }
                        auto &level = it->second;
                        const auto level_trades = level->match_order(
                                order, [this](const core::OrderId &order_id) -> void { f_orders.erase(order_id); });
                        trades.insert(trades.end(), std::make_move_iterator(level_trades.begin()),
                                      std::make_move_iterator(level_trades.end()));
                        if (level->empty()) {
                                it = opposite_levels.erase(it);
                        } else {
                                ++it;
                        }
                }

                if (order->quantity() > 0) {
                        const auto [level_it, inserted] = levels.try_emplace(
                                order->price(), std::make_unique<Level>(order->price(), f_trade_counter));
                        const auto &level = level_it->second;
                        f_orders[order->id()] = order.get();
                        level->add_order(std::move(order));
                }

                return trades;
        }
} // namespace orderbook
