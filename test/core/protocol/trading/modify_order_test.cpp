#include <gtest/gtest.h>

#include <core/protocol/trading/modify_order.h>

using namespace core;
using namespace protocol::trading;

TEST(SerDeTest, ModifyOrderRequest)
{
        constexpr auto expected_symbol = "test";
        constexpr auto expected_order_id = 123;
        constexpr auto expected_price = 122333444555;
        constexpr auto expected_quantity = 123456;

        const ModifyOrderRequest expected_request = {expected_symbol, expected_order_id, expected_price,
                                                     expected_quantity};

        const auto buffer = new unsigned char[expected_request.size()];
        expected_request.serialize(buffer);

        auto request = ModifyOrderRequest{};

        request.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(request.symbol(), expected_symbol);
        ASSERT_EQ(request.order_id(), expected_order_id);
        ASSERT_EQ(request.price(), expected_price);
        ASSERT_EQ(request.quantity(), expected_quantity);
}

TEST(SerDeTest, ModifyOrderResponse)
{
        constexpr auto expected_symbol = "test";
        constexpr auto expected_order_id = 123;
        constexpr auto expected_success = true;

        const ModifyOrderResponse expected_response = {expected_symbol, expected_order_id, expected_success};

        const auto buffer = new unsigned char[expected_response.size()];
        expected_response.serialize(buffer);

        auto response = ModifyOrderResponse{};
        response.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(response.symbol(), expected_symbol);
        ASSERT_EQ(response.order_id(), expected_order_id);
        ASSERT_EQ(response.success(), expected_success);
}
