#ifndef BOOK_H
#define BOOK_H

#include <boost/log/trivial.hpp>
#include <iterator>
#include <map>
#include <unordered_map>
#include <vector>

#include "../common/types.h"
#include "level.h"
#include "order.h"
#include "trade.h"

namespace orderbook
{
        struct Snapshot
        {
                std::vector<std::pair<common::Price, common::Quantity>> bids;
                std::vector<std::pair<common::Price, common::Quantity>> asks;
        };

        class Book
        {
        public:
                Book() = default;
                Book(const Book &book) = default;
                Book(Book &&book) = default;
                Book &operator=(const Book &book) = default;
                Book &operator=(Book &&book) = default;
                ~Book() = default;

                std::pair<common::OrderId, std::vector<Trade>> add_order(const common::Price &price,
                                                                         const common::Quantity &quantity,
                                                                         const common::Side &side,
                                                                         const common::Timestamp &timestamp)
                {
                        const auto order_id = ++f_order_counter;
                        const auto order = std::make_shared<Order>(order_id, price, quantity, side, timestamp);
                        return std::make_pair(order_id, add_order(order));
                }

                void remove_order(common::OrderId order);

                void modify_order(const Order &order);

                void modify_order(Order &&order);

                Snapshot snapshot() const
                {
                        auto bids = std::vector<std::pair<common::Price, common::Quantity>>{};
                        bids.reserve(f_bids.size());
                        for (const auto &[price, level]: f_bids) {
                                bids.emplace_back(price, level->quantity());
                        }
                        auto asks = std::vector<std::pair<common::Price, common::Quantity>>{};
                        asks.reserve(f_asks.size());
                        for (const auto &[price, level]: f_asks) {
                                asks.emplace_back(price, level->quantity());
                        }
                        return Snapshot{std::move(bids), std::move(asks)};
                }

        private:
                static bool buy_check(const common::Price level_price, const common::Price order_price)
                {
                        return level_price <= order_price;
                }

                static bool sell_check(const common::Price level_price, const common::Price order_price)
                {
                        return level_price >= order_price;
                }

                std::vector<Trade> add_order(const std::shared_ptr<Order> &order)
                {
                        BOOST_LOG_TRIVIAL(info) << "Book::add_order << " << (*order);

                        std::vector<Trade> trades = order->side() == common::Side::Buy
                                                            ? match(order, f_bids, f_asks, buy_check)
                                                            : match(order, f_asks, f_bids, sell_check);

                        return trades;
                }

                template<typename Compare1, typename Compare2, typename PriceCheck>
                std::vector<Trade> match(const std::shared_ptr<Order> &order,
                                         std::map<common::Price, std::unique_ptr<Level>, Compare1> &levels,
                                         std::map<common::Price, std::unique_ptr<Level>, Compare2> &opposite_levels,
                                         PriceCheck price_check)
                {
                        std::vector<Trade> trades{};

                        for (auto it = opposite_levels.begin(); it != opposite_levels.end() && order->quantity() > 0;) {
                                if (!price_check(it->first, order->price())) {
                                        break;
                                }
                                auto &level = it->second;
                                BOOST_LOG_TRIVIAL(info) << "Book::match << " << (*order) << " <-> " << (*level);
                                const auto level_trades =
                                        level->match_order(order, [this](const common::OrderId &order_id) -> void {
                                                f_orders.erase(order_id);
                                        });
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
                                level->add_order(order);
                                f_orders[order->id()] = order;
                        }

                        BOOST_LOG_TRIVIAL(info) << "Book::match << Trades: " << trades.size();

                        return trades;
                }

                std::unordered_map<common::OrderId, std::shared_ptr<Order>> f_orders{};
                std::map<common::Price, std::unique_ptr<Level>, std::greater<>> f_bids{};
                std::map<common::Price, std::unique_ptr<Level>, std::less<>> f_asks{};

                SharedCounter<common::OrderId> f_order_counter = SharedCounter<common::OrderId>();
                SharedCounter<common::TradeId> f_trade_counter = SharedCounter<common::TradeId>();
        };
} // namespace orderbook

#endif // BOOK_H
