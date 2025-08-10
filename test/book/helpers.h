#ifndef HELPERS_H
#define HELPERS_H

#include "../../src/common/types.h"
#include "../../src/book/order.h"

using namespace orderbook;
using namespace common;

inline std::shared_ptr<Order> create_order(OrderId order_id, Price price, Quantity quantity, Side side, Timestamp timestamp)
{
        return std::make_shared<Order>(order_id, price, quantity, side, timestamp);
}

template <typename T>
void pretty_print(std::vector<T> v)
{
        for (auto it = v.begin(); it != v.end(); ++it) {
                std::cout << *it << ", ";
        }
        std::cout << std::endl;
}

#endif //HELPERS_H
