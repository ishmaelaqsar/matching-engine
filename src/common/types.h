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

        enum class MessageType : uint8_t
        {
                Unknown = 0,
                Trade = 1,
                AddOrderRequest = 2,
                AddOrderResponse = 3,
                ModifyOrderRequest = 4,
                ModifyOrderResponse = 5,
                CancelOrderRequest = 6,
                CancelOrderResponse = 7,
                GetBookRequest = 8,
                GetBookResponse = 9,
        };

        inline std::ostream &operator<<(std::ostream &os, const MessageType &type)
        {
                switch (type) {
                        case MessageType::Trade: return os << "Trade";
                        case MessageType::AddOrderRequest: return os << "AddOrderRequest";
                        case MessageType::AddOrderResponse: return os << "AddOrderResponse";
                        case MessageType::ModifyOrderRequest: return os << "ModifyOrderRequest";
                        case MessageType::ModifyOrderResponse: return os << "ModifyOrderResponse";
                        case MessageType::CancelOrderRequest: return os << "CancelOrderRequest";
                        case MessageType::CancelOrderResponse: return os << "CancelOrderResponse";
                        case MessageType::GetBookRequest: return os << "GetBookRequest";
                        case MessageType::GetBookResponse: return os << "GetBookResponse";
                        case MessageType::Unknown:
                        default: return os << "Unknown";
                }
        }
} // namespace common

#endif // TYPES_H
