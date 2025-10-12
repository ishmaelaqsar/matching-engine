#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

namespace core
{
    using ConnectionId = uint64_t;
    using Username = std::string_view;

    using Symbol = std::string;
    using OrderId = uint64_t;
    using TradeId = uint64_t;
    using Timestamp = uint64_t;
    using Price = uint64_t;
    using Quantity = uint64_t;
    using StrLen = uint8_t;

    enum class Side : uint8_t {
        Unknown = 0,
        Buy = 1,
        Sell = 2,
    };

    inline std::ostream& operator<<(std::ostream& os, const Side& side)
    {
        switch (side) {
        case Side::Buy:
            return os << "Buy";
        case Side::Sell:
            return os << "Sell";
        case Side::Unknown:
        default:
            return os << "Unknown";
        }
    }
} // namespace core
