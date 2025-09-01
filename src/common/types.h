#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <string>

namespace common
{
        using Symbol = std::string;
        using ConnectionId = uint64_t;
        using OrderId = uint64_t;
        using TradeId = uint64_t;
        using Timestamp = uint64_t;
        using Price = uint64_t;
        using Quantity = uint64_t;
        using StrLen = uint8_t;

        enum class Side : uint8_t
        {
                Unknown = 0,
                Buy = 1,
                Sell = 2,
        };

        inline std::ostream &operator<<(std::ostream &os, const Side &side)
        {
                switch (side) {
                        case Side::Buy: return os << "Buy";
                        case Side::Sell: return os << "Sell";
                        case Side::Unknown:
                        default: return os << "Unknown";
                }
        }
} // namespace common

#endif // TYPES_H
