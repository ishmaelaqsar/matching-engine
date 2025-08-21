#ifndef LEVEL_H
#define LEVEL_H

#include <boost/log/trivial.hpp>
#include <chrono>
#include <vector>

#include "../common/types.h"
#include "order.h"
#include "shared_counter.h"
#include "trade.h"

namespace orderbook
{
        class Level
        {
        public:
                Level(const common::Price price, const SharedCounter<common::TradeId> &id_counter) :
                    f_price(price), f_id_counter(id_counter)
                {}

                Level(const Level &level) = delete;

                Level(Level &&level) = default;

                ~Level() = default;

                Level &operator=(const Level &level) = delete;

                Level &operator=(Level &&level) = default;

                friend std::ostream &operator<<(std::ostream &os, const Level &level);

                [[nodiscard]] bool empty() const;

                [[nodiscard]] common::Quantity quantity() const;

                void add_order(const std::shared_ptr<Order> &order);

                std::vector<Trade> match_order(const std::shared_ptr<Order> &order);

        private:
                common::Price f_price;
                SharedCounter<common::TradeId> f_id_counter;
                common::Quantity f_quantity = 0;
                std::vector<std::shared_ptr<Order>> f_orders;
        };

        inline std::ostream &operator<<(std::ostream &os, const Level &level)
        {
                os << "Level{";
                os << "price: " << level.f_price << ", ";
                os << "quantity: " << level.f_quantity << ", ";
                os << "orders: " << level.f_orders.size();
                os << "}";
                return os;
        }

        inline bool Level::empty() const
        {
                return f_quantity == 0;
        }

        inline common::Quantity Level::quantity() const
        {
                return f_quantity;
        }

        inline void Level::add_order(const std::shared_ptr<Order> &order)
        {
                f_orders.emplace_back(order);
                f_quantity += order->quantity();
        }

        inline std::vector<Trade> Level::match_order(const std::shared_ptr<Order> &order)
        {
                auto trades = std::vector<Trade>();
                auto now = std::chrono::system_clock::now().time_since_epoch().count();

                for (auto it = f_orders.begin(); it != f_orders.end() && order->quantity() > 0;) {
                        if (Order *target_order = it->get(); target_order->quantity() == order->quantity()) {
                                BOOST_LOG_TRIVIAL(debug) << "Level::match_order << Both orders filled: " << order->id()
                                                         << " == " << target_order->id();
                                const auto traded_qty = order->quantity();
                                trades.emplace_back(++f_id_counter, f_price, traded_qty, now, order->id(),
                                                    target_order->id());
                                f_quantity -= traded_qty;
                                order->set_quantity(0);
                                it = f_orders.erase(it);
                        } else if (target_order->quantity() < order->quantity()) {
                                BOOST_LOG_TRIVIAL(debug) << "Level::match_order << Level order filled: " << order->id()
                                                         << " > " << target_order->id();
                                const auto traded_qty = target_order->quantity();
                                trades.emplace_back(++f_id_counter, f_price, traded_qty, now, order->id(),
                                                    target_order->id());
                                f_quantity -= traded_qty;
                                order->set_quantity(order->quantity() - traded_qty);
                                it = f_orders.erase(it);
                        } else {
                                BOOST_LOG_TRIVIAL(debug)
                                        << "Level::match_order << Incoming order filled: " << order->id() << " < "
                                        << target_order->id();
                                const auto traded_qty = order->quantity();
                                trades.emplace_back(++f_id_counter, f_price, traded_qty, now, order->id(),
                                                    target_order->id());
                                f_quantity -= traded_qty;
                                order->set_quantity(0);
                                target_order->set_quantity(target_order->quantity() - traded_qty);
                                ++it;
                        }
                }

                return trades;
        }
} // namespace orderbook

#endif // LEVEL_H
