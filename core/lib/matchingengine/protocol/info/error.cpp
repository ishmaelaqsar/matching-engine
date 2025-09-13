#include <matchingengine/protocol/info/error.h>

#include <sstream>
#include <utility>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol::info
{

        std::ostream &operator<<(std::ostream &os, const ErrorCode &error)
        {
                switch (error) {
                        case ErrorCode::SERVER_ERROR: return os << "SERVER_ERROR";
                        case ErrorCode::CLIENT_ERROR: return os << "CLIENT_ERROR";
                        case ErrorCode::UNKNOWN_ERROR:
                        default: return os << "UNKNOWN_ERROR";
                }
        }

        Error::Error(const ErrorCode error_code) : f_error_code(error_code)
        {}

        Error::Error(const ErrorCode error_code, std::string message) :
            f_error_code(error_code), f_message(std::move(message))
        {}

        auto Error::server_error() -> Error
        {
                return Error{ErrorCode::SERVER_ERROR};
        }

        auto Error::server_error(std::string message) -> Error
        {
                return Error{ErrorCode::SERVER_ERROR, std::move(message)};
        }

        auto Error::client_error() -> Error
        {
                return Error{ErrorCode::CLIENT_ERROR};
        }

        auto Error::client_error(const std::string &message) -> Error
        {
                return Error{ErrorCode::CLIENT_ERROR, message};
        }

        auto Error::type() -> MessageType
        {
                return MessageType::Error;
        }

        auto Error::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_uint8(static_cast<uint8_t>(f_error_code), dst, &offset);
                serialize_string(f_message, dst, &offset);
        }

        auto Error::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_error_code = static_cast<ErrorCode>(deserialize_uint8(src, &offset));
                f_message = deserialize_string(src, &offset);
        }

        auto Error::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto Error::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "Error{";
                os << "error_code: " << f_error_code << ", ";
                os << "message: " << f_message;
                os << "}";
                return os.str();
        }

        auto Error::size() const -> size_t
        {
                return sizeof(f_error_code) + sizeof(StrLen) + f_message.size();
        }

        auto Error::error_code() const -> ErrorCode
        {
                return f_error_code;
        }

        auto Error::message() const -> const std::string &
        {
                return f_message;
        }
} // namespace core::protocol::info
