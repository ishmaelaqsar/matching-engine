#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::admin
{
    class LoginRequest final : public Message<LoginRequest>
    {
    public:
        LoginRequest() = default;
        LoginRequest(const LoginRequest& LoginRequest) = default;
        LoginRequest(LoginRequest&& LoginRequest) = default;
        LoginRequest& operator=(const LoginRequest& LoginRequest) = default;
        LoginRequest& operator=(LoginRequest&& LoginRequest) = default;
        ~LoginRequest() = default;

        explicit LoginRequest(const std::string& username);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] const std::string& username() const;

    private:
        std::string f_username{};
    };

    class LoginResponse final : public Message<LoginResponse>
    {
    public:
        LoginResponse() = default;
        LoginResponse(const LoginResponse& LoginResponse) = default;
        LoginResponse(LoginResponse&& LoginResponse) = default;
        LoginResponse& operator=(const LoginResponse& LoginResponse) = default;
        LoginResponse& operator=(LoginResponse&& LoginResponse) = default;
        ~LoginResponse() = default;

        LoginResponse(const std::string& username, bool success);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] const std::string& username() const;

        [[nodiscard]] bool success() const;

    private:
        std::string f_username{};
        bool f_success{};
    };
} // namespace core::protocol::admin
