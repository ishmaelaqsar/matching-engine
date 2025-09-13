#include <matchingengine/protocol/message.h>

#include <matchingengine/protocol/trading/cancel_order.h>

namespace core::protocol
{
        auto operator<<(std::ostream &os, const MessageType &type) -> std::ostream &
        {
                switch (type) {
                        case MessageType::Info: return os << "Info";
                        case MessageType::Error: return os << "Error";
                        case MessageType::Trade: return os << "Trade";
                        case MessageType::AddOrderRequest: return os << "AddOrderRequest";
                        case MessageType::AddOrderResponse: return os << "AddOrderResponse";
                        case MessageType::ModifyOrderRequest: return os << "ModifyOrderRequest";
                        case MessageType::ModifyOrderResponse: return os << "ModifyOrderResponse";
                        case MessageType::CancelOrderRequest: return os << "CancelOrderRequest";
                        case MessageType::CancelOrderResponse: return os << "CancelOrderResponse";
                        case MessageType::GetBookRequest: return os << "GetBookRequest";
                        case MessageType::GetBookResponse: return os << "GetBookResponse";
                        default: return os << "Unknown";
                }
        }
} // namespace core::protocol
