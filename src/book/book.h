#ifndef BOOK_H
#define BOOK_H
#include <map>
#include <stdexcept>
#include <vector>

#include "order.h"
#include "trade.h"

namespace orderbook
{
        class Book
        {
        public:
                Book() = default;

                Book(const Book &book) = default;

                Book(Book &&book) = default;

                Book &operator=(const Book &book) = default;

                Book &operator=(Book &&book) = default;

                ~Book() = default;

                void add_order(const Order &order);

                void add_order(Order &&order);

                void remove_order(OrderId order);

                void modify_order(const Order &order);

                void modify_order(Order &&order);

        private:
                std::unordered_map<OrderId, Order> f_orders = std::unordered_map<OrderId, Order>();
                std::map<Price, Order, std::greater<>> f_bids = std::map<Price, Order, std::greater<>>();
                std::map<Price, Order, std::less<>> f_asks = std::map<Price, Order, std::less<>>();

                std::vector<Trade> match_order(const Order &order);
        };

        inline void Book::add_order(const Order &order)
        {
                if (f_orders.find(order.id()) != f_orders.end()) {
                        throw std::invalid_argument("Order already exists");
                }

                f_orders[order.id()] = order;

                if (order.side() == Side::BUY) {
                        f_bids.push_back(order);
                } else {
                        f_asks.push_back(order);
                }
        }
}

#endif //BOOK_H
