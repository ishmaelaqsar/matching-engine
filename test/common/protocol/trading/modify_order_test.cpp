#include "common/protocol/trading/modify_order.h"

#include <gtest/gtest.h>

using namespace common;
using namespace protocol::trading;

TEST(SerDeTest, ModifyOrderRequest)
{
        constexpr auto expected_order_id = 123;
        constexpr auto expected_price = 122333444555;
        constexpr auto expected_quantity = 123456;

        const ModifyOrderRequest expected_request = {expected_order_id, expected_price, expected_quantity};

        const auto buffer = new char[expected_request.size()];
        ModifyOrderRequest::serialize(expected_request, buffer);

        const auto request = ModifyOrderRequest::deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(request.order_id(), expected_order_id);
        ASSERT_EQ(request.price(), expected_price);
        ASSERT_EQ(request.quantity(), expected_quantity);
}
