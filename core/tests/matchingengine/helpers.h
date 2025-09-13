#pragma once

#include <gtest/gtest.h>

#include <type_traits>

#include <matchingengine/types.h>

template<typename T, typename = void>
struct is_iterable : std::false_type
{};
template<typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T &>())), decltype(std::end(std::declval<T &>()))>>
    : std::true_type
{};
template<typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template<typename T>
struct is_map : std::false_type
{};
template<typename Key, typename Val, typename... Args>
struct is_map<std::map<Key, Val, Args...>> : std::true_type
{};
template<typename T>
constexpr bool is_map_v = is_map<T>::value;

template<typename Map, typename CheckFn>
void assert_map(const Map &expected, const Map &actual, CheckFn check);

template<typename T, typename CheckFn>
void assert_iterable(std::initializer_list<T> expected, const std::vector<T> &actual, CheckFn check)
{
        assert_iterable(std::vector<T>(expected), actual, check);
}

template<typename Iterable, typename CheckFn>
void assert_iterable(const Iterable &expected, const Iterable &actual, CheckFn check)
{
        ASSERT_EQ(std::distance(std::begin(expected), std::end(expected)),
                  std::distance(std::begin(actual), std::end(actual)));

        auto it_exp = std::begin(expected);
        auto it_act = std::begin(actual);

        using Elem = typename std::decay_t<Iterable>::value_type;

        for (; it_exp != std::end(expected); ++it_exp, ++it_act) {
                if constexpr (is_iterable_v<Elem> && !is_map_v<Elem>) { // Handle nested vectors, etc.
                        // Let template deduction figure out the types automatically.
                        assert_iterable(*it_exp, *it_act, check);
                } else if constexpr (is_map_v<Elem>) { // Handle nested maps
                        assert_map(*it_exp, *it_act, check);
                } else {
                        check(*it_exp, *it_act); // Base case
                }
        }
}

template<typename Map, typename CheckFn>
void assert_map(const Map &expected, const Map &actual, CheckFn check)
{
        using Value = typename Map::mapped_type;

        ASSERT_EQ(expected.size(), actual.size());

        for (const auto &[key, expected_val]: expected) {
                auto it = actual.find(key);
                ASSERT_TRUE(it != actual.end()) << "Key not found in actual map: " << key;
                const auto &actual_val = it->second;

                if constexpr (is_iterable_v<Value> && !is_map_v<Value>) {
                        assert_iterable(expected_val, actual_val, check);
                } else if constexpr (is_map_v<Value>) {
                        assert_map(expected_val, actual_val, check);
                } else {
                        check(expected_val, actual_val);
                }
        }
}
