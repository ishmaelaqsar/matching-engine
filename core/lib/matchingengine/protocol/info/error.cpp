#include <matchingengine/protocol/info/error.h>
#include <matchingengine/protocol/serialize_helper.h>
#include <sstream>
#include <utility>

namespace core::protocol::info
{

    std::ostream& operator<<(std::ostream& os, const ErrorCode& error)
    {
        switch (error) {
        case ErrorCode::SERVER_ERROR:
            return os << "SERVER_ERROR";
        case ErrorCode::CLIENT_ERROR:
            return os << "CLIENT_ERROR";
        case ErrorCode::UNKNOWN_ERROR:
        default:
            return os << "UNKNOWN_ERROR";
        }
    }

    Error::Error(const ErrorCode error_code) : f_error_code(error_code) {}

    Error::Error(const ErrorCode error_code, std::string&& message)
        : f_error_code(error_code), f_message(std::move(message))
    {
    }

    Error Error::server_error()
    {
        return Error{ErrorCode::SERVER_ERROR};
    }

    Error Error::server_error(std::string&& message)
    {
        return Error{ErrorCode::SERVER_ERROR, std::move(message)};
    }

    Error Error::client_error()
    {
        return Error{ErrorCode::CLIENT_ERROR};
    }

    Error Error::client_error(std::string&& message)
    {
        return Error{ErrorCode::CLIENT_ERROR, std::move(message)};
    }

    MessageType Error::type()
    {
        return MessageType::Error;
    }

    void Error::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_uint8(static_cast<uint8_t>(f_error_code), dst, &offset);
        serialize_string(f_message, dst, &offset);
    }

    void Error::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_error_code = static_cast<ErrorCode>(deserialize_uint8(src, &offset));
        f_message = deserialize_string(src, &offset);
    }

    void Error::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string Error::to_string() const
    {
        std::ostringstream os;
        os << "Error{";
        os << "error_code: " << f_error_code << ", ";
        os << "message: " << f_message;
        os << "}";
        return os.str();
    }

    size_t Error::size() const
    {
        return sizeof(f_error_code) + sizeof(StrLen) + f_message.size();
    }

    ErrorCode Error::error_code() const
    {
        return f_error_code;
    }

    const std::string& Error::message() const
    {
        return f_message;
    }
} // namespace core::protocol::info
