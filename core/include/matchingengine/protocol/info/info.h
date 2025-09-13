#pragma once

#include <matchingengine/protocol/message.h>

namespace core::protocol::info
{
        class Info final : public Message<Info>
        {
        public:
                Info() = default;
                Info(const Info &Info) = default;
                Info(Info &&Info) = default;
                Info &operator=(const Info &Info) = default;
                Info &operator=(Info &&Info) = default;
                ~Info() = default;

                explicit Info(const std::string &message);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto message() const -> const std::string &;

        private:
                std::string f_message;
        };
} // namespace core::protocol::info
