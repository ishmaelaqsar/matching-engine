#ifndef LEVEL_H
#define LEVEL_H
#include <vector>

#include "order.h"
#include "trade.h"
#include "types.h"
#include <chrono>

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

                explicit operator bool() const;

                void add_order(const Order &order);

                std::vector<Trade> match_order(Order &order);

        private:
                Price f_price;
                SharedCounter<TradeId> f_id_counter;
                Quantity f_quantity = 0;
                std::vector<Order> f_orders;
        };

        inline Level::operator bool() const
        {
                return this->f_quantity > 0;
        }

        inline void Level::add_order(const Order &order)
        {
                f_orders.emplace_back(order);
        }

        inline std::vector<Trade> Level::match_order(Order &order)
        {
                auto trades = std::vector<Trade>();
                auto now = std::chrono::system_clock::now().time_since_epoch().count();

                for (auto it = f_orders.begin(); it != f_orders.end() && order.quantity() > 0; ++it) {
                        if (it->quantity() == order.quantity()) {
                                trades.emplace_back(++f_id_counter, f_price, it->quantity(), now,
                                                    order.id(), it->id());
                                order.set_quantity(0);
                                it = f_orders.erase(it);
                        } else if (it->quantity() < order.quantity()) {
                                trades.emplace_back(++f_id_counter, f_price, it->quantity(), now,
                                                    order.id(), it->id());
                                order.set_quantity(order.quantity() - it->quantity());
                                it = f_orders.erase(it);
                        } else {
                                trades.emplace_back(++f_id_counter, f_price, order.quantity(), now,
                                                    order.id(), it->id());
                                order.set_quantity(0);
                                it->set_quantity(it->quantity() - order.quantity());
                        }
                }

                return trades;
        }
}

#endif //LEVEL_H
