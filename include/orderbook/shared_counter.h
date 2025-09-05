#pragma once

#include <memory>
#include <type_traits>

// NOT THREAD SAFE
template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class SharedCounter
{
public:
        SharedCounter(const SharedCounter &other) = default;
        SharedCounter(SharedCounter &&other) = default;
        SharedCounter &operator=(const SharedCounter &other) = default;
        SharedCounter &operator=(SharedCounter &&other) = default;
        ~SharedCounter() = default;

        SharedCounter() : f_counter(std::make_shared<T>(0))
        {}

        T &operator++()
        {
                return ++(*f_counter);
        }

        T &operator--()
        {
                return --(*f_counter);
        }

        T &operator*() const
        {
                return *f_counter;
        }

        explicit operator bool() const
        {
                return f_counter != nullptr;
        }

private:
        std::shared_ptr<T> f_counter;
};
