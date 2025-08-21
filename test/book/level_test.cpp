#include <gtest/gtest.h>

#include "../../src/book/level.h"
#include "../../src/common/types.h"
#include "../helpers.h"

using namespace orderbook;
using namespace common;

TEST(BookTest, LevelAddOrder)
{
        const auto counter = SharedCounter<TradeId>();
        const auto order = create_order(1, 123, 456, Side::Buy, 1);

        auto level = Level(123, counter);

        ASSERT_TRUE(level.empty());

        level.add_order(order);

        ASSERT_FALSE(level.empty());
        ASSERT_EQ(level.quantity(), 456);
}

TEST(BookTest, LevelMatchOrder)
{
        const auto counter = SharedCounter<TradeId>();
        constexpr auto price = 123;
        const auto order_buy_1 = create_order(1, price, 100, Side::Buy, 1);
        const auto order_buy_2 = create_order(2, price, 100, Side::Buy, 2);

        auto level = Level(price, counter);

        level.add_order(order_buy_1);
        level.add_order(order_buy_2);

        ASSERT_EQ(level.quantity(), 200);

        auto order_sell = create_order(2, price, 150, Side::Sell, 3);

        const auto trades = level.match_order(order_sell);

        ASSERT_EQ(trades.size(), 2);

        ASSERT_EQ(trades[0].id(), 1);
        ASSERT_EQ(trades[0].price(), price);
        ASSERT_EQ(trades[0].quantity(), 100);
        ASSERT_EQ(trades[0].source_order(), order_sell->id());
        ASSERT_EQ(trades[0].matched_order(), order_buy_1->id());

        ASSERT_EQ(trades[1].id(), 2);
        ASSERT_EQ(trades[1].price(), price);
        ASSERT_EQ(trades[1].quantity(), 50);
        ASSERT_EQ(trades[1].source_order(), order_sell->id());
        ASSERT_EQ(trades[1].matched_order(), order_buy_2->id());

        ASSERT_EQ(level.quantity(), 50);
        ASSERT_EQ(order_sell->quantity(), 0);
}
