#include "core/protocol/message.h"

#include <memory>

#include "core/protocol/trading/add_order.h"
#include "core/protocol/trading/modify_order.h"
#include "core/protocol/trading/trade.h"
#include "core/protocol/view/get_book.h"

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

        std::unique_ptr<Message> create(const MessageType &type)
        {
                switch (type) {
                        case MessageType::Trade: return std::unique_ptr<trading::Trade>();
                        case MessageType::AddOrderRequest: return std::unique_ptr<trading::AddOrderRequest>();
                        case MessageType::AddOrderResponse: return std::unique_ptr<trading::AddOrderResponse>();
                        case MessageType::ModifyOrderRequest: return std::unique_ptr<trading::ModifyOrderRequest>();
                        case MessageType::ModifyOrderResponse: return std::unique_ptr<trading::ModifyOrderResponse>();
                        case MessageType::GetBookRequest: return std::unique_ptr<view::GetBookRequest>();
                        case MessageType::GetBookResponse: return std::unique_ptr<view::GetBookResponse>();
                        default: throw std::runtime_error("Unknown MessageType");
                }
        }
} // namespace core::protocol
