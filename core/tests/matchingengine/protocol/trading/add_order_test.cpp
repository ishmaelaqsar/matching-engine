#include <gtest/gtest.h>

#include <matchingengine/protocol/trading/add_order.h>

using namespace core;
using namespace protocol::trading;

TEST(SerDeTest, AddOrderRequest)
{
        constexpr auto expected_symbol = "EUR/USD";
        constexpr auto expected_price = 122333444555;
        constexpr auto expected_quantity = 123456;
        constexpr auto expected_side = Side::Buy;

        const AddOrderRequest expected_request = {expected_symbol, expected_price, expected_quantity, expected_side};

        const auto buffer = new unsigned char[expected_request.size()];
        expected_request.serialize(buffer);

        auto request = AddOrderRequest{};
        request.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(request.symbol(), expected_symbol);
        ASSERT_EQ(request.price(), expected_price);
        ASSERT_EQ(request.quantity(), expected_quantity);
        ASSERT_EQ(request.side(), expected_side);
}

TEST(SerDeTest, AddOrderResponse)
{
        constexpr auto expected_symbol = "EUR/USD";
        constexpr auto expected_order_id = 1;
        constexpr auto expected_timestamp = 122333444555;

        const AddOrderResponse expected_response = {expected_symbol, expected_order_id, expected_timestamp};

        const auto buffer = new unsigned char[expected_response.size()];
        expected_response.serialize(buffer);

        auto response = AddOrderResponse{};
        response.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(response.symbol(), expected_symbol);
        ASSERT_EQ(response.order_id(), expected_order_id);
        ASSERT_EQ(response.timestamp(), expected_timestamp);
}
