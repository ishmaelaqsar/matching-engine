#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>

namespace common::protocol
{
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

        class Message
        {
        public:
                virtual ~Message() = default;
                virtual void serialize(unsigned char *dst) const = 0;
                virtual void deserialize(const unsigned char *src) = 0;
                virtual void print(std::ostream &os) const = 0;
                [[nodiscard]] virtual size_t size() const = 0;
                [[nodiscard]] virtual MessageType type() const = 0;
        };

        inline std::ostream &operator<<(std::ostream &os, const Message &msg) {
                msg.print(os);
                return os;
        }
} // namespace common::protocol

#endif // MESSAGE_H
