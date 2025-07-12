#include <gtest/gtest.h>

#include "../../src/common/add_order.h"
#include "../../src/common/header.h"

using namespace common;

TEST(SerDeTest, AddOrderRequest)
{
        constexpr auto expected_symbol = "EUR/USD";
        constexpr auto expected_price = 122333444555;
        constexpr auto expected_quantity = 123456;
        constexpr auto expected_side = Side::Buy;

        char buffer[AddOrderRequest::SIZE];
        AddOrderRequest::serialize({expected_symbol, expected_price, expected_quantity, expected_side}, buffer);

        const auto [symbol, price, quantity, side] = AddOrderRequest::deserialize(buffer);

        ASSERT_EQ(symbol, expected_symbol);
        ASSERT_EQ(price, expected_price);
        ASSERT_EQ(quantity, expected_quantity);
        ASSERT_EQ(side, expected_side);
}

TEST(SerDeTest, AddOrderResponse)
{
        constexpr auto expected_order_id = 1;
        constexpr auto expected_timestamp = 122333444555;

        char buffer[AddOrderResponse::SIZE];
        AddOrderResponse::serialize({expected_order_id, expected_timestamp}, buffer);

        const auto [order_id, timestamp] = AddOrderResponse::deserialize(buffer);

        ASSERT_EQ(expected_order_id, order_id);
        ASSERT_EQ(expected_timestamp, timestamp);
}
