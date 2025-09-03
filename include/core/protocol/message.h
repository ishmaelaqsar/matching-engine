#pragma once

#include <memory>

namespace core::protocol
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

        std::ostream &operator<<(std::ostream &os, const MessageType &type);

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

        std::ostream &operator<<(std::ostream &os, const Message &msg);

        std::shared_ptr<Message> create(const MessageType& type);
} // namespace core::protocol
