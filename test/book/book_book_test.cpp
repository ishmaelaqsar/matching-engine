#include <gtest/gtest.h>

#include "helpers.h"
#include "../../src/common/types.h"
#include "../../src/book/book.h"

using namespace orderbook;
using namespace common;

TEST(BookTest, BookBuyTrades)
{
        auto book = Book();

        // buy 10@100 in an empty book
        {
                const auto trades = book.add_order(100, 10, Side::Buy, 1);
                ASSERT_TRUE(trades.empty());
        }

        // sell 5@100 should match the previous buy
        {
                const auto trades = book.add_order(100, 5, Side::Sell, 2);
                ASSERT_EQ(trades.size(), 1);
                ASSERT_EQ(trades[0].id(), 100);
        }
}