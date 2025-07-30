#include <gtest/gtest.h>
#include <climits>

#include "../../src/common/util.h"
#include "../../src/common/types.h"

using namespace common;

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
