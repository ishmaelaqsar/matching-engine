#include <matchingengine/protocol/admin/login.h>

#include <sstream>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol::admin
{

        LoginRequest::LoginRequest(const std::string &username) : f_username(username)
        {}

        auto LoginRequest::type() -> MessageType
        {
                return MessageType::LoginRequest;
        }

        auto LoginRequest::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_username, dst, &offset);
        }

        auto LoginRequest::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_username = deserialize_string(src, &offset);
        }

        auto LoginRequest::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto LoginRequest::to_string() const -> std::string
        {
                std::stringstream os;
                os << "LoginRequest{username: " << f_username << "}";
                return os.str();
        }

        auto LoginRequest::size() const -> size_t
        {
                return sizeof(StrLen) + f_username.length();
        }

        auto LoginRequest::username() const -> const std::string &
        {
                return f_username;
        }

        LoginResponse::LoginResponse(const std::string &username, const bool success) :
            f_username(username), f_success(success)
        {}

        auto LoginResponse::type() -> MessageType
        {
                return MessageType::LoginResponse;
        }

        auto LoginResponse::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_username, dst, &offset);
                serialize_uint8(f_success, dst, &offset);
        }

        auto LoginResponse::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_username = deserialize_string(src, &offset);
                f_success = static_cast<bool>(deserialize_uint8(src, &offset));
        }

        auto LoginResponse::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto LoginResponse::to_string() const -> std::string
        {
                std::stringstream os;
                os << "LoginResponse{username: " << f_username << ", success: " << f_success << "}";
                return os.str();
        }

        auto LoginResponse::size() const -> size_t
        {
                return sizeof(StrLen) + f_username.length() + sizeof(f_success);
        }

        auto LoginResponse::username() const -> const std::string &
        {
                return f_username;
        }

        auto LoginResponse::success() const -> bool
        {
                return f_success;
        }
} // namespace core::protocol::admin
