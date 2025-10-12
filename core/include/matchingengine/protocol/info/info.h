#pragma once

#include <matchingengine/protocol/message.h>

namespace core::protocol::info
{
    class Info final : public Message<Info>
    {
    public:
        Info() = default;
        Info(const Info& Info) = default;
        Info(Info&& Info) = default;
        Info& operator=(const Info& Info) = default;
        Info& operator=(Info&& Info) = default;
        ~Info() = default;

        explicit Info(const std::string& message);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] const std::string& message() const;

    private:
        std::string f_message;
    };
} // namespace core::protocol::info
