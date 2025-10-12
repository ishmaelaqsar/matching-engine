#pragma once

#include <matchingengine/protocol/message.h>

namespace core::protocol::info
{
    enum class ErrorCode : uint8_t {
        UNKNOWN_ERROR = 0,
        SERVER_ERROR = 1,
        CLIENT_ERROR = 2,
    };

    std::ostream& operator<<(std::ostream& os, const ErrorCode& error);

    class Error final : public Message<Error>
    {
    public:
        Error() = default;
        Error(const Error& Error) = default;
        Error(Error&& Error) = default;
        Error& operator=(const Error& Error) = default;
        Error& operator=(Error&& Error) = default;
        ~Error() = default;

        explicit Error(ErrorCode error_code);

        Error(ErrorCode error_code, std::string&& message);

        static Error server_error();

        static Error server_error(std::string&& message);

        static Error client_error();

        static Error client_error(std::string&& message);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] ErrorCode error_code() const;

        [[nodiscard]] const std::string& message() const;

    private:
        ErrorCode f_error_code = ErrorCode::UNKNOWN_ERROR;
        std::string f_message;
    };
} // namespace core::protocol::info
