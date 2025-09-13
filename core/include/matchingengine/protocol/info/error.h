#pragma once

#include <matchingengine/protocol/message.h>

namespace core::protocol::info
{
        enum class ErrorCode : uint8_t
        {
                UNKNOWN_ERROR = 0,
                SERVER_ERROR = 1,
                CLIENT_ERROR = 2,
        };

        std::ostream &operator<<(std::ostream &os, const ErrorCode &error);

        class Error final : public Message<Error>
        {
        public:
                Error() = default;
                Error(const Error &Error) = default;
                Error(Error &&Error) = default;
                Error &operator=(const Error &Error) = default;
                Error &operator=(Error &&Error) = default;
                ~Error() = default;

                explicit Error(ErrorCode error_code);

                Error(ErrorCode error_code, std::string message);

                static auto server_error() -> Error;

                static auto server_error(std::string message) -> Error;

                static auto client_error() -> Error;

                static auto client_error(const std::string &message) -> Error;

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto error_code() const -> ErrorCode;

                [[nodiscard]] auto message() const -> const std::string &;

        private:
                ErrorCode f_error_code = ErrorCode::UNKNOWN_ERROR;
                std::string f_message;
        };
} // namespace core::protocol::info
