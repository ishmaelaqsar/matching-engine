#include <gtest/gtest.h>
#include <matchingengine/orderbook/level.h>
#include <matchingengine/types.h>

using namespace orderbook;
using namespace core;

static void add_order(
    Level& level, OrderId order_id, Price price, Quantity quantity, Side side,
    Timestamp timestamp
)
{
    auto order =
        std::make_unique<Order>(order_id, price, quantity, side, timestamp);
    level.add_order(std::move(order));
}

TEST(BookTest, LevelAddOrder)
{
    const auto counter = SharedCounter<TradeId>();

    auto level = Level(123, counter);

    ASSERT_TRUE(level.empty());

    add_order(level, 1, 123, 456, Side::Buy, 1);

    ASSERT_FALSE(level.empty());
    ASSERT_EQ(level.quantity(), 456);
}

TEST(BookTest, LevelMatchOrder)
{
    const auto counter = SharedCounter<TradeId>();
    constexpr auto price = 123;

    auto level = Level(price, counter);

    add_order(level, 1, price, 100, Side::Buy, 1);
    add_order(level, 2, price, 100, Side::Buy, 2);

    ASSERT_EQ(level.quantity(), 200);

    auto order_sell = std::make_unique<Order>(2, price, 150, Side::Sell, 3);
    auto filled = false;

    const auto trades =
        level.match_order(order_sell, [&filled](const OrderId&) {
            filled = true;
        });

    ASSERT_EQ(trades.size(), 2);

    ASSERT_EQ(trades[0].id(), 1);
    ASSERT_EQ(trades[0].price(), price);
    ASSERT_EQ(trades[0].quantity(), 100);
    ASSERT_EQ(trades[0].source_order(), order_sell->id());
    ASSERT_EQ(trades[0].matched_order(), 1);

    ASSERT_EQ(trades[1].id(), 2);
    ASSERT_EQ(trades[1].price(), price);
    ASSERT_EQ(trades[1].quantity(), 50);
    ASSERT_EQ(trades[1].source_order(), order_sell->id());
    ASSERT_EQ(trades[1].matched_order(), 2);

    ASSERT_EQ(level.quantity(), 50);
    ASSERT_EQ(order_sell->quantity(), 0);
    ASSERT_TRUE(filled);
}
