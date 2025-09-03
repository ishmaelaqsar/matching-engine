#include <orderbook/book.h>
#include <orderbook/order.h>

namespace orderbook
{
        std::pair<core::OrderId, std::vector<Trade>> Book::add_order(const core::Price &price,
                                                                     const core::Quantity &quantity,
                                                                     const core::Side &side,
                                                                     const core::Timestamp &timestamp)
        {
                const auto order_id = ++f_order_counter;
                const auto order = std::make_shared<Order>(order_id, price, quantity, side, timestamp);
                auto trades = order->side() == core::Side::Buy ? match(order, f_bids, f_asks, buy_check)
                                                               : match(order, f_asks, f_bids, sell_check);
                return std::make_pair(order_id, std::move(trades));
        }

        bool Book::modify_order(const core::OrderId order_id, const core::Price &price, const core::Quantity &quantity)
        {
                const auto it = f_orders.find(order_id);
                if (it == f_orders.end()) {
                        return false;
                }
                if (const auto &order = it->second; order->price() != price) {
                        remove_order(order_id);
                        add_order(price, quantity, order->side(), order->timestamp());
                } else {
                        const auto level = get_level(order);
                        level->set_quantity(level->quantity() - order->quantity() + quantity);
                        order->set_quantity(quantity);
                }
                return true;
        }

        bool Book::remove_order(core::OrderId order_id)
        {
                const auto it = f_orders.find(order_id);
                if (it == f_orders.end()) {
                        return false;
                }
                f_orders.erase(it);
                const auto order = it->second;
                const auto level = get_level(order);
                level->remove_order(order);
                return true;
        }

        std::shared_ptr<Level> Book::get_level(const std::shared_ptr<Order> &order)
        {
                if (order->side() == core::Side::Buy) {
                        const auto it = f_bids.find(order->price());
                        if (it == f_bids.end()) {
                                return nullptr;
                        }
                        return it->second;
                }

                const auto it = f_asks.find(order->price());
                if (it == f_asks.end()) {
                        return nullptr;
                }
                return it->second;
        }

        Snapshot Book::snapshot() const
        {
                auto bids = std::vector<std::pair<core::Price, core::Quantity>>{};
                bids.reserve(f_bids.size());
                for (const auto &[price, level]: f_bids) {
                        bids.emplace_back(price, level->quantity());
                }

                auto asks = std::vector<std::pair<core::Price, core::Quantity>>{};
                asks.reserve(f_asks.size());
                for (const auto &[price, level]: f_asks) {
                        asks.emplace_back(price, level->quantity());
                }

                return Snapshot{std::move(bids), std::move(asks)};
        }
} // namespace orderbook
