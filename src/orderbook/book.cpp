#include "orderbook/book.h"
#include "orderbook/order.h"

namespace orderbook
{
        inline std::pair<core::OrderId, std::vector<Trade>> Book::add_order(const   core::Price &price,
                                                                              const core::Quantity &quantity,
                                                                              const core::Side &side,
                                                                              const core::Timestamp &timestamp)
        {
                const auto order_id = ++f_order_counter;
                const auto order = std::make_shared<Order>(order_id, price, quantity, side, timestamp);
                return std::make_pair(order_id, add_order(order));
        }

        inline std::vector<Trade> Book::add_order(const std::shared_ptr<Order> &order)
        {
                BOOST_LOG_TRIVIAL(info) << "Book::add_order << " << (*order);

                std::vector<Trade> trades = order->side() == core::Side::Buy
                                                    ? match(order, f_bids, f_asks, buy_check)
                                                    : match(order, f_asks, f_bids, sell_check);

                return trades;
        }

        inline Snapshot Book::snapshot() const
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
}