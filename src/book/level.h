#ifndef LEVEL_H
#define LEVEL_H
#include <chrono>
#include <vector>

#include "order.h"
#include "trade.h"
#include "types.h"

#include "shared_counter.h"

namespace orderbook
{
        class Level
        {
        public:
                Level(const Price price, const SharedCounter<TradeId> &id_counter) :
                        f_price(price),
                        f_id_counter(id_counter) {}

                Level(const Level &level) = delete;

                Level(Level &&level) = default;

                ~Level() = default;

                Level &operator=(const Level &level) = delete;

                Level &operator=(Level &&level) = default;

                [[nodiscard]] bool empty() const;

                [[nodiscard]] Quantity quantity() const;

                void add_order(std::shared_ptr<Order> &order);

                std::vector<Trade> match_order(const std::shared_ptr<Order> &order);

        private:
                Price f_price;
                SharedCounter<TradeId> f_id_counter;
                Quantity f_quantity = 0;
                std::vector<std::shared_ptr<Order>> f_orders;
        };

        inline bool Level::empty() const
        {
                return f_quantity == 0;
        }

        inline Quantity Level::quantity() const
        {
                return f_quantity;
        }

        inline void Level::add_order(std::shared_ptr<Order> &order)
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
                                const auto traded_qty = order->quantity();
                                trades.emplace_back(++f_id_counter, f_price, traded_qty, now, order->id(),
                                                    target_order->id());
                                f_quantity -= traded_qty;
                                order->set_quantity(0);
                                it = f_orders.erase(it);
                        } else if (target_order->quantity() < order->quantity()) {
                                const auto traded_qty = target_order->quantity();
                                trades.emplace_back(++f_id_counter, f_price, traded_qty, now, order->id(),
                                                    target_order->id());
                                f_quantity -= traded_qty;
                                order->set_quantity(order->quantity() - traded_qty);
                                it = f_orders.erase(it);
                        } else {
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
}

#endif //LEVEL_H
