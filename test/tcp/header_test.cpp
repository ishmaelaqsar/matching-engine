#include <gtest/gtest.h>

#include "../../src/tcp/header.h"

using namespace common;

TEST(SerDeTest, HeaderAddOrderRequest)
{
        constexpr auto expected_type = MessageType::AddOrderRequest;
        constexpr auto expected_length = 32;

        char buffer[MessageHeader::Size];
        MessageHeader::serialize({expected_type, expected_length}, buffer);

        const auto [type, length] = MessageHeader::deserialize(buffer);

        ASSERT_EQ(expected_type, type);
        ASSERT_EQ(expected_length, length);
}

TEST(SerDeTest, HeaderAddOrderResponse)
{
        constexpr auto expected_type = MessageType::AddOrderResponse;
        constexpr auto expected_length = 32;

        char buffer[MessageHeader::Size];
        MessageHeader::serialize({expected_type, expected_length}, buffer);

        const auto [type, length] = MessageHeader::deserialize(buffer);

        ASSERT_EQ(expected_type, type);
        ASSERT_EQ(expected_length, length);
}
