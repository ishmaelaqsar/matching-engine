#pragma once

#include <gtest/gtest.h>

#include <matchingengine/types.h>

template<typename T>
static auto pretty_print(std::vector<T> v) -> void
{
        for (auto it = v.begin(); it != v.end(); ++it) {
                std::cout << *it << ", ";
        }
        std::cout << std::endl;
}

template<typename T>
static auto assert_iterable(std::vector<T> expected, std::vector<T> actual) -> void
{
        ASSERT_EQ(expected.size(), actual.size());
        for (auto i = 0; i < expected.size(); ++i) {
                ASSERT_EQ(expected[i], actual[i]);
        }
}

template<typename T>
static auto assert_iterable(std::initializer_list<T> expected, std::vector<T> actual, std::function<void(T, T)> check)
        -> void
{
        ASSERT_EQ(expected.size(), actual.size());
        size_t i = 0;
        for (auto it = expected.begin(); it != expected.end(); ++it, ++i) {
                check(*it, actual[i]);
        }
}
