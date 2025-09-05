#include <gtest/gtest.h>

#include <core/protocol/header.h>

using namespace core;
using namespace protocol;

TEST(SerDeTest, HeaderAddOrderRequest)
{
        constexpr auto expected_type = MessageType::AddOrderRequest;
        constexpr auto expected_length = 32;

        unsigned char buffer[Header::Size];
        Header::serialize({expected_type, expected_length}, buffer);

        const auto [type, length] = Header::deserialize(buffer);

        ASSERT_EQ(expected_type, type);
        ASSERT_EQ(expected_length, length);
}

TEST(SerDeTest, HeaderAddOrderResponse)
{
        constexpr auto expected_type = MessageType::AddOrderResponse;
        constexpr auto expected_length = 32;

        unsigned char buffer[Header::Size];
        Header::serialize({expected_type, expected_length}, buffer);

        const auto [type, length] = Header::deserialize(buffer);

        ASSERT_EQ(expected_type, type);
        ASSERT_EQ(expected_length, length);
}
