#include <matchingengine/orderbook/level.h>

#include <boost/chrono/chrono.hpp>

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

        auto Level::quantity() const -> core::Quantity
        {
                return f_quantity;
        }

        auto Level::set_quantity(const core::Quantity quantity) -> void
        {
                f_quantity = quantity;
        }

        auto Level::add_order(const std::shared_ptr<Order> &order) -> void
        {
                f_orders.emplace_back(order);
                f_quantity += order->quantity();
        }

        auto Level::remove_order(const std::shared_ptr<Order> &order) -> void
        {
                f_orders.erase(std::remove(f_orders.begin(), f_orders.end(), order));
                f_quantity -= order->quantity();
        }

        auto Level::match_order(const std::shared_ptr<Order> &order, const OrderCallback &order_fill_callback)
                -> std::vector<Trade>
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
                                order_fill_callback(target_order->id());
                        } else if (target_order->quantity() < order->quantity()) {
                                BOOST_LOG_TRIVIAL(debug) << "Level::match_order << Level order filled: " << order->id()
                                                         << " > " << target_order->id();
                                const auto traded_qty = target_order->quantity();
                                trades.emplace_back(++f_id_counter, f_price, traded_qty, now, order->id(),
                                                    target_order->id());
                                f_quantity -= traded_qty;
                                order->set_quantity(order->quantity() - traded_qty);
                                it = f_orders.erase(it);
                                order_fill_callback(target_order->id());
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
