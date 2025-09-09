#include <matchingengine/orderbook/book.h>

#include <matchingengine/orderbook/order.h>

namespace orderbook
{
        auto Book::add_order(const core::Price &price, const core::Quantity &quantity, const core::Side &side,
                             const core::Timestamp &timestamp) -> std::pair<core::OrderId, std::vector<Trade>>
        {
                const auto order_id = ++f_order_counter;
                auto order = std::make_unique<Order>(order_id, price, quantity, side, timestamp);

                auto trades = order->side() == core::Side::Buy ? match(std::move(order), f_bids, f_asks, buy_check)
                                                               : match(std::move(order), f_asks, f_bids, sell_check);

                return std::make_pair(order_id, std::move(trades));
        }

        auto Book::modify_order(const core::OrderId &order_id, const core::Quantity &quantity,
                                const core::Timestamp &timestamp) -> bool
        {
                const auto it = f_orders.find(order_id);
                if (it == f_orders.end()) {
                        return false;
                }

                if (quantity == 0) {
                        remove_order(order_id);
                        return true;
                }

                const auto order = it->second;
                const auto [level, cleanup_function] = get_level(*order);
                level->remove_quantity(order->quantity());
                level->add_quantity(quantity);
                order->set_quantity(quantity, timestamp);
                return true;
        }

        auto Book::remove_order(const core::OrderId &order_id) -> bool
        {
                const auto it = f_orders.find(order_id);
                if (it == f_orders.end()) {
                        return false;
                }

                const auto order = it->second;
                f_orders.erase(it);

                const auto [level, cleanup] = get_level(*order);
                level->remove_order(order->id());
                if (level->empty()) {
                        cleanup();
                }

                return true;
        }

        auto Book::snapshot() const -> Snapshot
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

        auto Book::buy_check(const core::Price level_price, const core::Price order_price) -> bool
        {
                return level_price <= order_price;
        }

        auto Book::sell_check(const core::Price level_price, const core::Price order_price) -> bool
        {
                return level_price >= order_price;
        }

        auto Book::get_level(const Order &order) -> std::pair<Level *, LevelRemoval>
        {
                if (order.side() == core::Side::Buy) {
                        auto it = f_bids.find(order.price());
                        if (it == f_bids.end()) {
                                return {nullptr, [] {}};
                        }
                        return {it->second.get(), [this, it] { f_bids.erase(it); }};
                }

                auto it = f_asks.find(order.price());
                if (it == f_asks.end()) {
                        return {nullptr, [] {}};
                }
                return {it->second.get(), [this, it] { f_asks.erase(it); }};
        }

        auto Book::add_order(const core::OrderId &order_id, const core::Price &price, const core::Quantity &quantity,
                             const core::Side &side, const core::Timestamp &timestamp) -> std::vector<Trade>
        {
                auto order = std::make_unique<Order>(order_id, price, quantity, side, timestamp);
                return order->side() == core::Side::Buy ? match(std::move(order), f_bids, f_asks, buy_check)
                                                        : match(std::move(order), f_asks, f_bids, sell_check);
        }
} // namespace orderbook
