#ifndef BOOK_H
#define BOOK_H
#include <map>
#include <stdexcept>
#include <vector>

#include "level.h"
#include "order.h"
#include "trade.h"

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

                std::vector<Trade> add_order(const Price &price, const Quantity &quantity, const Side &side,
                                             const Timestamp &timestamp);

                void remove_order(OrderId order);

                void modify_order(const Order &order);

                void modify_order(Order &&order);

        private:
                std::unordered_map<OrderId, std::shared_ptr<Order>> f_orders =
                        std::unordered_map<OrderId, std::shared_ptr<Order>>();
                std::map<Price, std::unique_ptr<Level>, std::greater<>> f_bids =
                        std::map<Price, std::unique_ptr<Level>, std::greater<>>();
                std::map<Price, std::unique_ptr<Level>, std::less<>> f_asks =
                        std::map<Price, std::unique_ptr<Level>, std::less<>>();

                SharedCounter<OrderId> f_order_counter = SharedCounter<OrderId>();
                SharedCounter<TradeId> f_trade_counter = SharedCounter<TradeId>();

                std::vector<Trade> add_order(const std::shared_ptr<Order> &order);
        };

        inline std::vector<Trade> Book::add_order(const Price &price, const Quantity &quantity, const Side &side,
                                                  const Timestamp &timestamp)
        {
                const auto order_id = ++f_order_counter;
                const auto order = std::make_shared<Order>(order_id, price, quantity, side, timestamp);
                return add_order(order);
        }

        inline std::vector<Trade> Book::add_order(const std::shared_ptr<Order> &order)
        {
                std::vector<Trade> trades{};

                // todo only matches the level with exact price but should also match other levels if they have a "better" price e.g. asks at lower price if order is a bid
                auto match = [&](auto &levels, auto &opposite_levels) {
                        if (const auto level = opposite_levels.find(order->price());
                                level != opposite_levels.end() && level->second && !level->second->empty()) {
                                trades = level->second->match_order(order);
                                if (level->second->empty()) {
                                        opposite_levels.erase(level);
                                }
                        }
                        if (order->quantity() > 0) {
                                const auto &level = levels.try_emplace(order->price(),
                                                                       std::make_unique<Level>(
                                                                               order->price(),
                                                                               f_trade_counter)).first->second;
                                level->add_order(order);
                                f_orders[order->id()] = order;
                        }
                };

                if (order->side() == Side::BUY) {
                        match(f_bids, f_asks);
                } else {
                        match(f_asks, f_bids);
                }

                return trades;
        }
}

#endif //BOOK_H
