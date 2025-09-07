#pragma once

#include <memory>
#include <string>

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

        auto operator<<(std::ostream &os, const MessageType &type) -> std::ostream &;

        template<typename SubMessage>
        class Message
        {
        public:
                auto serialize(unsigned char *dst) const -> void
                {
                        static_cast<const SubMessage *>(this)->serialize(dst);
                }

                auto deserialize(const unsigned char *src) -> void
                {
                        static_cast<SubMessage *>(this)->deserialize(src);
                }

                auto to_string() const -> std::string
                {
                        return static_cast<const SubMessage *>(this)->to_string();
                }

                auto print(std::ostream &os) const -> void
                {
                        static_cast<const SubMessage *>(this)->print(os);
                }

                [[nodiscard]] auto size() const -> size_t
                {
                        return static_cast<const SubMessage *>(this)->size();
                }

                [[nodiscard]] auto type() const -> MessageType
                {
                        return static_cast<MessageType>(static_cast<const SubMessage *>(this)->type());
                }

        protected:
                Message() = default;
        };

        template<typename SubMessage>
        auto operator<<(std::ostream &os, const Message<SubMessage> &msg) -> std::ostream &
        {
                msg.print(os);
                return os;
        }
} // namespace core::protocol
