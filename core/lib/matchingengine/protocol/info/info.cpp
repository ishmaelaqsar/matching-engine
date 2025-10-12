#include <matchingengine/protocol/info/info.h>
#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/types.h>
#include <sstream>

namespace core::protocol::info
{

    Info::Info(const std::string& message) : f_message(message) {}

    MessageType Info::type()
    {
        return MessageType::Info;
    }

    void Info::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_message, dst, &offset);
    }

    void Info::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_message = deserialize_string(src, &offset);
    }

    void Info::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string Info::to_string() const
    {
        std::ostringstream os;
        os << "Info{";
        os << "message: " << f_message;
        os << "}";
        return os.str();
    }

    size_t Info::size() const
    {
        return sizeof(StrLen) + f_message.size();
    }

    const std::string& Info::message() const
    {
        return f_message;
    }
} // namespace core::protocol::info
