#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::admin
{
        class LoginRequest final : public Message<LoginRequest>
        {
        public:
                LoginRequest() = default;
                LoginRequest(const LoginRequest &LoginRequest) = default;
                LoginRequest(LoginRequest &&LoginRequest) = default;
                LoginRequest &operator=(const LoginRequest &LoginRequest) = default;
                LoginRequest &operator=(LoginRequest &&LoginRequest) = default;
                ~LoginRequest() = default;

                explicit LoginRequest(const std::string &username);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto username() const -> const std::string &;

        private:
                std::string f_username{};
        };

        class LoginResponse final : public Message<LoginResponse>
        {
        public:
                LoginResponse() = default;
                LoginResponse(const LoginResponse &LoginResponse) = default;
                LoginResponse(LoginResponse &&LoginResponse) = default;
                LoginResponse &operator=(const LoginResponse &LoginResponse) = default;
                LoginResponse &operator=(LoginResponse &&LoginResponse) = default;
                ~LoginResponse() = default;

                LoginResponse(const std::string &username, bool success);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto username() const -> const std::string &;

                [[nodiscard]] auto success() const -> bool;

        private:
                std::string f_username{};
                bool f_success{};
        };
} // namespace core::protocol::admin
