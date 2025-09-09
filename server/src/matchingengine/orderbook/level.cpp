#include <matchingengine/orderbook/level.h>

#include <chrono>

namespace orderbook
{
        Level::Level(const core::Price price, const SharedCounter<core::TradeId> &id_counter) :
            f_price(price), f_id_counter(id_counter)
        {}

        auto operator<<(std::ostream &os, const Level &level) -> std::ostream &
        {
                os << "Level{";
                os << "price: " << level.f_price << ", ";
                os << "quantity: " << level.f_quantity << ", ";
                os << "orders: " << level.f_orders.size();
                os << "}";
                return os;
        }

        auto Level::empty() const -> bool
        {
                return f_quantity == 0;
        }

        auto Level::price() const -> core::Price
        {
                return f_price;
        }

        auto Level::quantity() const -> core::Quantity
        {
                return f_quantity;
        }

        auto Level::add_quantity(const core::Quantity quantity) -> void
        {
                f_quantity += quantity;
        }

        auto Level::remove_quantity(const core::Quantity quantity) -> void
        {
                f_quantity -= quantity;
        }

        auto Level::add_order(std::unique_ptr<Order> order) -> void
        {
                add_quantity(order->quantity());
                f_orders.push_back(std::move(order));
        }

        auto Level::remove_order(const core::OrderId &id) -> void
        {
                for (auto it = f_orders.begin(); it != f_orders.end(); ++it) {
                        if ((*it)->id() == id) {
                                remove_quantity((*it)->quantity());
                                f_orders.erase(it);
                                return;
                        }
                }
        }

        auto Level::match_order(const std::unique_ptr<Order> &incoming, const OrderCallback &on_fill)
                -> std::vector<Trade>
        {
                std::vector<Trade> trades;
                const auto now =
                        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
                                .time_since_epoch()
                                .count();

                while (!f_orders.empty() && incoming->quantity() > 0) {
                        const auto &target = f_orders.front();
                        auto trade_qty = std::min(incoming->quantity(), target->quantity());

                        trades.emplace_back(++f_id_counter, f_price, trade_qty, now, incoming->id(), target->id());

                        incoming->remove_quantity(trade_qty);
                        target->remove_quantity(trade_qty);
                        remove_quantity(trade_qty);

                        if (target->quantity() == 0) {
                                on_fill(target->id());
                                f_orders.pop_front();
                        }
                }

                return trades;
        }
} // namespace orderbook
