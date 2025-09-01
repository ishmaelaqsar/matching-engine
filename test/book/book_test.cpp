#include <gtest/gtest.h>

#include "../../include/core/types.h"
#include "../../include/orderbook/book.h"

using namespace orderbook;
using namespace common;

TEST(BookTest, BookTrades)
{
        auto book = Book();

        // 0 | 0

        // buy 10@100 in an empty orderbook
        {
                const auto trades = book.add_order(100, 10, Side::Buy, 1);
                ASSERT_TRUE(trades.empty());
        }

        //  10 @ 100 | 0

        // sell 5@100 should match the previous buy
        {
                const auto trades = book.add_order(100, 5, Side::Sell, 2);
                ASSERT_EQ(trades.size(), 1);
                ASSERT_EQ(trades[0].id(), 1);
                ASSERT_EQ(trades[0].price(), 100);
                ASSERT_EQ(trades[0].quantity(), 5);
        }

        // 5 @ 100 | 0

        // sell 10@101 should not match anything
        {
                const auto trades = book.add_order(101, 10, Side::Sell, 3);
                ASSERT_TRUE(trades.empty());
        }

        // 5 @ 100 | 10 @ 101

        // sell 20@102 should not match anything
        {
                const auto trades = book.add_order(102, 20, Side::Sell, 4);
                ASSERT_TRUE(trades.empty());
        }

        // 5 @ 100 | 10 @ 101
        //         | 20 @ 102

        // buy 40@103 should fill all sell orders and have 10 buys left over
        {
                const auto trades = book.add_order(103, 40, Side::Buy, 5);
                ASSERT_EQ(trades.size(), 2);

                ASSERT_EQ(trades[0].id(), 2);
                ASSERT_EQ(trades[0].price(), 101);
                ASSERT_EQ(trades[0].quantity(), 10);

                ASSERT_EQ(trades[1].id(), 3);
                ASSERT_EQ(trades[1].price(), 102);
                ASSERT_EQ(trades[1].quantity(), 20);
        }

        // 10 @ 103 | 0
        //  5 @ 100 |
}