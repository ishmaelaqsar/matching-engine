#include <gtest/gtest.h>

#include "../../../../src/common/protocol/view/get_book.h"
#include "../../../../src/tcp/header.h"
#include "../../../helpers.h"

using namespace common;
using namespace protocol::view;

TEST(SerDeTest, GetBookRequest)
{
        constexpr auto expected_symbol = "AAPL";

        const auto expected_request = GetBookRequest{expected_symbol};

        const auto buffer = new unsigned char[expected_request.size()];
        expected_request.serialize(buffer);

        auto request = GetBookRequest{};
        request.deserialize(buffer);
        delete[] buffer;

        ASSERT_EQ(request.symbol(), expected_symbol);
}

TEST(SerDeTest, GetBookResponse)
{
        const auto expected_bids = {Level{100, 10}, Level{99, 10}, Level{98, 10}};
        const auto expected_asks = {Level{101, 10}};

        const auto expected_response = GetBookResponse{expected_bids, expected_asks};

        const auto buffer = new unsigned char[expected_response.size()];
        expected_response.serialize(buffer);

        auto response = GetBookResponse{};
        response.deserialize(buffer);
        delete[] buffer;

        assert_iterable<Level>(expected_bids, response.bids(), [](const Level l1, const Level l2) -> void {
                ASSERT_EQ(l1.price, l2.price);
                ASSERT_EQ(l1.quantity, l2.quantity);
        });
}
