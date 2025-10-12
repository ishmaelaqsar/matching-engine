#include <matchingengine/protocol/admin/login.h>
#include <matchingengine/protocol/serialize_helper.h>
#include <sstream>

namespace core::protocol::admin
{

    LoginRequest::LoginRequest(const std::string& username)
        : f_username(username)
    {
    }

    MessageType LoginRequest::type()
    {
        return MessageType::LoginRequest;
    }

    void LoginRequest::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_username, dst, &offset);
    }

    void LoginRequest::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_username = deserialize_string(src, &offset);
    }

    void LoginRequest::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string LoginRequest::to_string() const
    {
        std::stringstream os;
        os << "LoginRequest{username: " << f_username << "}";
        return os.str();
    }

    size_t LoginRequest::size() const
    {
        return sizeof(StrLen) + f_username.length();
    }

    const std::string& LoginRequest::username() const
    {
        return f_username;
    }

    LoginResponse::LoginResponse(
        const std::string& username, const bool success
    )
        : f_username(username), f_success(success)
    {
    }

    MessageType LoginResponse::type()
    {
        return MessageType::LoginResponse;
    }

    void LoginResponse::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_username, dst, &offset);
        serialize_uint8(f_success, dst, &offset);
    }

    void LoginResponse::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_username = deserialize_string(src, &offset);
        f_success = static_cast<bool>(deserialize_uint8(src, &offset));
    }

    void LoginResponse::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string LoginResponse::to_string() const
    {
        std::stringstream os;
        os << "LoginResponse{username: " << f_username
           << ", success: " << f_success << "}";
        return os.str();
    }

    size_t LoginResponse::size() const
    {
        return sizeof(StrLen) + f_username.length() + sizeof(f_success);
    }

    const std::string& LoginResponse::username() const
    {
        return f_username;
    }

    bool LoginResponse::success() const
    {
        return f_success;
    }
} // namespace core::protocol::admin
