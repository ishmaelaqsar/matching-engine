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

#endif //HELPERS_H
