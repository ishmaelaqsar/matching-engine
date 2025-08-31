#include <gtest/gtest.h>

#include "../../../src/common/protocol/header.h"

using namespace common;
using namespace common::protocol;

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

TEST(SerDeTest, HeaderError)
{
        unsigned char buffer[Header::Size];
        EXPECT_THROW(
                Header::serialize({MessageType::AddOrderRequest, std::numeric_limits<uint16_t>::max() + 1}, buffer),
                std::length_error
                );
}
