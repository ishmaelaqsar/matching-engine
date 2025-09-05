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

        std::vector<Trade> Book::add_order(const core::OrderId &order_id, const core::Price &price,
                                           const core::Quantity &quantity, const core::Side &side,
                                           const core::Timestamp &timestamp)
        {
                const auto order = std::make_shared<Order>(order_id, price, quantity, side, timestamp);
                return order->side() == core::Side::Buy ? match(order, f_bids, f_asks, buy_check)
                                                        : match(order, f_asks, f_bids, sell_check);
        }

        std::pair<bool, std::vector<Trade>> Book::modify_order(const core::OrderId &order_id, const core::Price &price,
                                                               const core::Quantity &quantity,
                                                               const core::Timestamp &timestamp)
        {
                const auto it = f_orders.find(order_id);
                if (it == f_orders.end()) {
                        return {false, {}};
                }
                if (const auto &order = it->second; order->price() != price) {
                        remove_order(order_id);
                        auto trades = add_order(order_id, price, quantity, order->side(), timestamp);
                        return std::make_pair(true, std::move(trades));
                } else {
                        const auto [level, cleanup_function] = get_level(order);
                        level->set_quantity(level->quantity() - order->quantity() + quantity);
                        order->set_quantity(quantity);
                        return {true, {}};
                }
        }

        bool Book::remove_order(const core::OrderId &order_id)
        {
                const auto it = f_orders.find(order_id);
                if (it == f_orders.end()) {
                        return false;
                }
                f_orders.erase(it);
                const auto order = it->second;
                const auto [level, cleanup] = get_level(order);
                level->remove_order(order);
                if (level->empty()) {
                        cleanup();
                }
                return true;
        }

        std::pair<std::shared_ptr<Level>, Book::LevelRemoval> Book::get_level(const std::shared_ptr<Order> &order)
        {
                std::shared_ptr<Level> level;
                LevelRemoval removal;

                if (order->side() == core::Side::Buy) {
                        const auto it = f_bids.find(order->price());
                        if (it == f_bids.end()) {
                                return {nullptr, [] {}};
                        }
                        level = it->second;
                        removal = [this, it]() -> void { this->f_bids.erase(it); };
                } else {
                        const auto it = f_asks.find(order->price());
                        if (it == f_asks.end()) {
                                return {nullptr, [] {}};
                        }
                        level = it->second;
                        removal = [this, it]() -> void { this->f_asks.erase(it); };
                }

                return {level, removal};
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
