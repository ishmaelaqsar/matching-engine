#pragma once

#include <cstdint>
#include <string>

namespace core::protocol
{
    enum class MessageType : uint8_t {
        Unknown,
        LoginRequest,
        LoginResponse,
        LogoutRequest,
        LogoutResponse,
        Info,
        Error,
        Trade,
        AddOrderRequest,
        AddOrderResponse,
        ModifyOrderRequest,
        ModifyOrderResponse,
        CancelOrderRequest,
        CancelOrderResponse,
        GetBookRequest,
        GetBookResponse,
        GetOrdersRequest,
        GetOrdersResponse,
    };

    std::ostream& operator<<(std::ostream& os, const MessageType& type);
    std::string operator+(const std::string& s, MessageType type);

    template <typename SubMessage> class Message
    {
    public:
        void serialize(unsigned char* dst) const
        {
            static_cast<const SubMessage*>(this)->serialize(dst);
        }

        void deserialize(const unsigned char* src)
        {
            static_cast<SubMessage*>(this)->deserialize(src);
        }

        void print(std::ostream& os) const
        {
            static_cast<const SubMessage*>(this)->print(os);
        }

        [[nodiscard]] std::string to_string() const
        {
            return static_cast<const SubMessage*>(this)->to_string();
        }

        [[nodiscard]] size_t size() const
        {
            return static_cast<const SubMessage*>(this)->size();
        }

        [[nodiscard]] MessageType type() const
        {
            return static_cast<MessageType>(
                static_cast<const SubMessage*>(this)->type()
            );
        }

    protected:
        Message() = default;
    };

    template <typename SubMessage>
    std::ostream& operator<<(std::ostream& os, const Message<SubMessage>& msg)
    {
        msg.print(os);
        return os;
    }
} // namespace core::protocol
