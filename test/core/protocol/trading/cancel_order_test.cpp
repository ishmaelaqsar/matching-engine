#include <gtest/gtest.h>

#include <core/protocol/trading/cancel_order.h>

using namespace core;
using namespace protocol::trading;

TEST(SerDeTest, CancelOrderRequest)
{
        constexpr auto expected_symbol = "test";
        constexpr auto expected_order_id = 123;

        const CancelOrderRequest expected_request = {expected_symbol, expected_order_id};

        const auto buffer = new unsigned char[expected_request.size()];
        expected_request.serialize(buffer);

        auto request = CancelOrderRequest{};

        request.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(request.symbol(), expected_symbol);
        ASSERT_EQ(request.order_id(), expected_order_id);
}

TEST(SerDeTest, CancelOrderResponse)
{
        constexpr auto expected_symbol = "test";
        constexpr auto expected_order_id = 123;
        constexpr auto expected_success = true;

        const CancelOrderResponse expected_response = {expected_symbol, expected_order_id, expected_success};

        const auto buffer = new unsigned char[expected_response.size()];
        expected_response.serialize(buffer);

        auto response = CancelOrderResponse{};
        response.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(response.symbol(), expected_symbol);
        ASSERT_EQ(response.order_id(), expected_order_id);
        ASSERT_EQ(response.success(), expected_success);
}
