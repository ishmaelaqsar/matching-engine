#pragma once

#include <gtest/gtest.h>

#include <core/types.h>
#include <orderbook/order.h>

using namespace orderbook;
using namespace core;

inline std::shared_ptr<Order> create_order(OrderId order_id, Price price, Quantity quantity, Side side,
                                           Timestamp timestamp)
{
        return std::make_shared<Order>(order_id, price, quantity, side, timestamp);
}

template<typename T>
void pretty_print(std::vector<T> v)
{
        for (auto it = v.begin(); it != v.end(); ++it) {
                std::cout << *it << ", ";
        }
        std::cout << std::endl;
}

template<typename T>
void assert_iterable(std::vector<T> expected, std::vector<T> actual)
{
        ASSERT_EQ(expected.size(), actual.size());
        for (auto i = 0; i < expected.size(); ++i) {
                ASSERT_EQ(expected[i], actual[i]);
        }
}

template<typename T>
void assert_iterable(std::initializer_list<T> expected, std::vector<T> actual, std::function<void(T, T)> check)
{
        ASSERT_EQ(expected.size(), actual.size());
        auto i = 0;
        for (auto it = expected.begin(); it != expected.end(); ++it, ++i) {
                check(*it, actual[i]);
        }
}
