#include <gtest/gtest.h>
#include <climits>

#include "../../src/tcp/protocol/types.h"

using namespace tcp;

TEST(SerDeTest, VarString)
{
        const std::string expected = "hello world";
        const auto buffer = new char[expected.length()];
        EXPECT_EQ(sizeof(StrLen) + expected.length(), serialize_string(expected, buffer, 0));
        size_t offset = 0;
        const std::string actual = deserialize_string(buffer, &offset);
        EXPECT_EQ(sizeof(StrLen) + expected.length(), offset);
        EXPECT_STREQ(expected.data(), actual.data());
        delete[] buffer;
}

TEST(SerDeTest, VarStringEmpty)
{
        const std::string expected{};
        const auto buffer = new char[expected.length()];
        EXPECT_EQ(sizeof(StrLen) + expected.length(), serialize_string(expected, buffer, 0));
        size_t offset = 0;
        const std::string actual = deserialize_string(buffer, &offset);
        EXPECT_EQ(sizeof(StrLen) + expected.length(), offset);
        EXPECT_STREQ(expected.data(), actual.data());
        delete[] buffer;
}

TEST(SerDeTest, VarStringOverflow)
{
        const auto expected = std::string(USHRT_MAX + 1, 'a');
        EXPECT_THROW(serialize_string(expected, nullptr, 0), std::length_error);
}

TEST(SerDeTest, FixedString)
{
        const std::string expected = "eur/usd";
        const auto buffer = new char[expected.length()];
        EXPECT_EQ(expected.length(), serialize_string(expected, buffer, 0, expected.length()));
        size_t offset = 0;
        const std::string actual = deserialize_string(buffer, &offset, expected.length());
        EXPECT_EQ(expected.length(), offset);
        EXPECT_STREQ(expected.data(), actual.data());
        delete[] buffer;
}

TEST(SerDeTest, FixedStringEmpty)
{
        const std::string expected{};
        const auto buffer = new char[expected.length()];
        EXPECT_EQ(expected.length(), serialize_string(expected, buffer, 0, expected.length()));
        size_t offset = 0;
        const std::string actual = deserialize_string(buffer, &offset, expected.length());
        EXPECT_EQ(expected.length(), offset);
        EXPECT_STREQ(expected.data(), actual.data());
        delete[] buffer;
}

TEST(SerDeTest, FixedStringOverflow)
{
        const auto expected = std::string(USHRT_MAX + 1, 'a');
        EXPECT_THROW(serialize_string(expected, nullptr, 0, USHRT_MAX), std::length_error);
}

TEST(SerDeTest, FixedStringTrim)
{
        const std::string expected = "eur/usd";
        const auto buffer = new char[expected.length()];
        EXPECT_EQ(expected.length(), serialize_string(expected, buffer, 0, expected.length()));
        size_t offset = 0;
        const std::string actual = deserialize_string(buffer, &offset, 3);
        EXPECT_EQ(3, offset);
        EXPECT_STREQ("eur", actual.data());
        delete[] buffer;
}
