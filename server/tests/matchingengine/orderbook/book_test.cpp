#include <gtest/gtest.h>
#include <matchingengine/helpers.h>
#include <matchingengine/orderbook/book.h>
#include <matchingengine/types.h>

using namespace orderbook;
using namespace core;

TEST(BookTest, BookAdd)
{
    auto book = Book();

    // 0 | 0

    // buy 10@100 in an empty orderbook
    {
        const auto [order_id, trades] = book.add_order(100, 10, Side::Buy, 1);
        ASSERT_EQ(order_id, 1);
        ASSERT_TRUE(trades.empty());
    }

    //  10 @ 100 | 0

    // sell 5@100 should match the previous buy
    {
        const auto [order_id, trades] = book.add_order(100, 5, Side::Sell, 2);
        ASSERT_EQ(order_id, 2);
        ASSERT_EQ(trades.size(), 1);
        ASSERT_EQ(trades[0].id(), 1);
        ASSERT_EQ(trades[0].price(), 100);
        ASSERT_EQ(trades[0].quantity(), 5);
    }

    // 5 @ 100 | 0

    // sell 10@101 should not match anything
    {
        const auto [order_id, trades] = book.add_order(101, 10, Side::Sell, 3);
        ASSERT_EQ(order_id, 3);
        ASSERT_TRUE(trades.empty());
    }

    // 5 @ 100 | 10 @ 101

    // sell 20@102 should not match anything
    {
        const auto [order_id, trades] = book.add_order(102, 20, Side::Sell, 4);
        ASSERT_EQ(order_id, 4);
        ASSERT_TRUE(trades.empty());
    }

    // 5 @ 100 | 10 @ 101
    //         | 20 @ 102

    // buy 40@103 should fill all sell orders and have 10 buys left over
    {
        const auto [order_id, trades] = book.add_order(103, 40, Side::Buy, 5);
        ASSERT_EQ(order_id, 5);
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

    const auto [bids, asks] = book.snapshot();

    const auto expected_bids = {
        std::make_pair<Price, Quantity>(103, 10),
        std::make_pair<Price, Quantity>(100, 5)
    };
    assert_iterable<std::pair<Price, Quantity>>(
        expected_bids, bids,
        [](const std::pair<Price, Quantity>& p1,
           const std::pair<Price, Quantity>& p2) -> void {
            ASSERT_EQ(p1.first, p2.first);
            ASSERT_EQ(p1.second, p2.second);
        }
    );

    ASSERT_TRUE(asks.empty());
}

TEST(BookTest, BookModify)
{
    auto book = Book();

    // 0 | 0

    // buy 10@100 in an empty orderbook
    {
        const auto [order_id, trades] = book.add_order(100, 10, Side::Buy, 1);
        ASSERT_EQ(order_id, 1);
        ASSERT_TRUE(trades.empty());
    }

    //  10 @ 100 | 0

    // sell 10@101 should not match anything
    {
        const auto [order_id, trades] = book.add_order(101, 10, Side::Sell, 2);
        ASSERT_EQ(order_id, 2);
        ASSERT_TRUE(trades.empty());
    }

    // 10 @ 100 | 10 @ 101

    // modify 10@100 -> 5@100
    {
        const auto success = book.modify_order(1, 5, 3);
        ASSERT_TRUE(success);
    }

    // 5 @ 100 | 10 @ 101

    // sell 5@100 should fill all buys
    {
        const auto [success, trades] = book.add_order(100, 5, Side::Sell, 4);
        ASSERT_TRUE(success);
        ASSERT_EQ(trades.size(), 1);

        ASSERT_EQ(trades[0].id(), 1);
        ASSERT_EQ(trades[0].price(), 100);
        ASSERT_EQ(trades[0].quantity(), 5);
    }

    // 0 | 10 @ 101

    const auto [bids, asks] = book.snapshot();

    ASSERT_TRUE(bids.empty());

    const auto expected_asks = {std::make_pair<Price, Quantity>(101, 10)};
    assert_iterable<std::pair<Price, Quantity>>(
        expected_asks, asks,
        [](const std::pair<Price, Quantity>& p1,
           const std::pair<Price, Quantity>& p2) -> void {
            ASSERT_EQ(p1.first, p2.first);
            ASSERT_EQ(p1.second, p2.second);
        }
    );
}

TEST(BookTest, BookCancel)
{
    auto book = Book();

    // 0 | 0

    // buy 10@100 in an empty orderbook
    {
        const auto [order_id, trades] = book.add_order(100, 10, Side::Buy, 1);
        ASSERT_EQ(order_id, 1);
        ASSERT_TRUE(trades.empty());
    }

    //  10 @ 100 | 0

    // cancel
    {
        const auto success = book.remove_order(1);
        ASSERT_TRUE(success);
    }

    // 0 | 0

    const auto [bids, asks] = book.snapshot();

    ASSERT_TRUE(bids.empty());
    ASSERT_TRUE(asks.empty());
}
