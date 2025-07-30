#ifndef BOOK_H
#define BOOK_H

#include <map>
#include <vector>

#include "level.h"
#include "order.h"
#include "trade.h"
#include "../common/types.h"

namespace orderbook
{
        class Book
        {
        public:
                Book() = default;

                Book(const Book &book) = default;

                Book(Book &&book) = default;

                Book &operator=(const Book &book) = default;

                Book &operator=(Book &&book) = default;

                ~Book() = default;

                std::vector<Trade> add_order(const common::Price &price, const common::Quantity &quantity,
                                             const common::Side &side,
                                             const common::Timestamp &timestamp);

                void remove_order(common::OrderId order);

                void modify_order(const Order &order);

                void modify_order(Order &&order);

        private:
                std::unordered_map<common::OrderId, std::shared_ptr<Order>> f_orders =
                        std::unordered_map<common::OrderId, std::shared_ptr<Order>>();
                std::map<common::Price, std::unique_ptr<Level>, std::greater<>> f_bids =
                        std::map<common::Price, std::unique_ptr<Level>, std::greater<>>();
                std::map<common::Price, std::unique_ptr<Level>, std::less<>> f_asks =
                        std::map<common::Price, std::unique_ptr<Level>, std::less<>>();

                SharedCounter<common::OrderId> f_order_counter = SharedCounter<common::OrderId>();
                SharedCounter<common::TradeId> f_trade_counter = SharedCounter<common::TradeId>();

                std::vector<Trade> add_order(const std::shared_ptr<Order> &order);

                template<typename Compare1, typename Compare2>
                std::vector<Trade> match(const std::shared_ptr<Order> &order,
                                         std::map<common::Price, std::unique_ptr<Level>, Compare1> &levels,
                                         std::map<common::Price, std::unique_ptr<Level>, Compare2> &opposite_levels);
        };

        inline std::vector<Trade> Book::add_order(const common::Price &price, const common::Quantity &quantity,
                                                  const common::Side &side,
                                                  const common::Timestamp &timestamp)
        {
                const auto order_id = ++f_order_counter;
                const auto order = std::make_shared<Order>(order_id, price, quantity, side, timestamp);
                return add_order(order);
        }

        inline std::vector<Trade> Book::add_order(const std::shared_ptr<Order> &order)
        {
                std::vector<Trade> trades = order->side() == common::Side::Buy
                                            ? trades = match(order, f_bids, f_asks)
                                            : trades = match(order, f_asks, f_bids);

                return trades;
        }

        template<typename Compare1, typename Compare2>
        std::vector<Trade> Book::match(const std::shared_ptr<Order> &order,
                                       std::map<common::Price, std::unique_ptr<Level>, Compare1> &levels,
                                       std::map<common::Price, std::unique_ptr<Level>, Compare2> &opposite_levels)
        {
                std::vector<Trade> trades{};

                auto it = opposite_levels.upper_bound(order->price());
                while (it != opposite_levels.end() && order->quantity() > 0) {
                        auto &level = it->second;
                        const auto level_trades = level->match_order(order);
                        trades.insert(trades.end(),
                                      std::make_move_iterator(level_trades.begin()),
                                      std::make_move_iterator(level_trades.end()));
                        if (level->empty()) {
                                it = opposite_levels.erase(it);
                                level.reset();
                        } else {
                                ++it;
                        }
                }

                if (order->quantity() > 0) {
                        const auto [level_it, inserted] = levels.try_emplace(
                                order->price(),
                                std::make_unique<Level>(order->price(), f_trade_counter)
                        );
                        const auto &level = level_it->second;
                        level->add_order(order);
                        f_orders[order->id()] = order;
                }

                return trades;
        }
}

#endif //BOOK_H
