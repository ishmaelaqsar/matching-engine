#include <gtest/gtest.h>

#include <matchingengine/helpers.h>
#include <matchingengine/protocol/view/get_orders.h>

using namespace core;
using namespace protocol::view;

TEST(SerDeTest, GetOrdersRequest)
{
        constexpr auto expected_username = "test";

        const auto expected_request = GetOrdersRequest{expected_username};

        const auto buffer = new unsigned char[expected_request.size()];
        expected_request.serialize(buffer);

        auto request = GetOrdersRequest{};
        request.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(request.username(), expected_username);
}

TEST(SerDeTest, GetOrdersResponse)
{
        const std::unordered_map<Symbol, std::vector<Order>> expected_orders = {
                {"AAPL",
                 {
                         Order{1, 100, 10, Side::Buy, 1},
                         Order{2, 100, 10, Side::Sell, 2},
                 }},
                {"GBP/USD", {Order{1, 2, 100000, Side::Buy, 1}}}};

        const auto expected_response = GetOrdersResponse{expected_orders};

        const auto buffer = new unsigned char[expected_response.size()];
        expected_response.serialize(buffer);

        auto response = GetOrdersResponse{};
        response.deserialize(buffer);
        delete[] buffer;

        assert_map<decltype(expected_orders)>(expected_orders, response.orders(),
                                              [](const Order &o1, const Order &o2) -> void {
                                                      ASSERT_EQ(o1.id, o2.id);
                                                      ASSERT_EQ(o1.price, o2.price);
                                                      ASSERT_EQ(o1.quantity, o2.quantity);
                                                      ASSERT_EQ(o1.timestamp, o2.timestamp);
                                              });
}
