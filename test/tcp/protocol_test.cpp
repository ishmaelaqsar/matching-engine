#include <gtest/gtest.h>

#include "../../src/tcp/protocol/header.h"

TEST(HeaderTest, VarStringSerDe)
{
        const std::string expected = "hello world";
        const auto buffer = new char[expected.length()];
        EXPECT_EQ(sizeof(orderbook::StrLen) + expected.length(), orderbook::serialize_string(expected, buffer, 0));
        size_t offset = 0;
        const std::string actual = orderbook::deserialize_string(buffer, &offset);
        EXPECT_EQ(sizeof(orderbook::StrLen) + expected.length(), offset);
        EXPECT_STREQ(expected.data(), actual.data());
        delete[] buffer;
}

TEST(HeaderTest, FixedStringSerDe)
{
        const std::string expected = "eur/usd";
        const auto buffer = new char[expected.length()];
        EXPECT_EQ(expected.length(), orderbook::serialize_string(expected, buffer, 0, expected.length()));
        size_t offset = 0;
        const std::string actual = orderbook::deserialize_string(buffer, &offset, expected.length());
        EXPECT_EQ(expected.length(), offset);
        EXPECT_STREQ(expected.data(), actual.data());
        delete[] buffer;
}
