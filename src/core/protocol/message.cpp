#include <memory>

#include <core/protocol/message.h>
#include <core/protocol/trading/add_order.h>
#include <core/protocol/trading/modify_order.h>
#include <core/protocol/trading/trade.h>
#include <core/protocol/view/get_book.h>

#include "core/protocol/trading/cancel_order.h"

namespace core::protocol
{
        std::ostream &operator<<(std::ostream &os, const MessageType &type)
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
                        default: return os << "Unknown";
                }
        }

        std::ostream &operator<<(std::ostream &os, const Message &msg)
        {
                msg.print(os);
                return os;
        }

        std::shared_ptr<Message> create(const MessageType &type)
        {
                switch (type) {
                        case MessageType::Trade: return std::make_shared<trading::Trade>();
                        case MessageType::AddOrderRequest: return std::make_shared<trading::AddOrderRequest>();
                        case MessageType::AddOrderResponse: return std::make_shared<trading::AddOrderResponse>();
                        case MessageType::ModifyOrderRequest: return std::make_shared<trading::ModifyOrderRequest>();
                        case MessageType::ModifyOrderResponse: return std::make_shared<trading::ModifyOrderResponse>();
                        case MessageType::CancelOrderRequest: return std::make_shared<trading::CancelOrderRequest>();
                        case MessageType::CancelOrderResponse: return std::make_shared<trading::CancelOrderResponse>();
                        case MessageType::GetBookRequest: return std::make_shared<view::GetBookRequest>();
                        case MessageType::GetBookResponse: return std::make_shared<view::GetBookResponse>();
                        default: throw std::runtime_error("Unknown MessageType");
                }
        }
} // namespace core::protocol
