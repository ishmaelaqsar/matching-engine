#include <matchingengine/protocol/info/info.h>

#include <sstream>

#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/types.h>

namespace core::protocol::info
{

        Info::Info(const std::string &message) : f_message(message)
        {}

        auto Info::type() -> MessageType
        {
                return MessageType::Info;
        }

        auto Info::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_message, dst, &offset);
        }

        auto Info::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_message = deserialize_string(src, &offset);
        }

        auto Info::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto Info::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "Info{";
                os << "message: " << f_message;
                os << "}";
                return os.str();
        }

        auto Info::size() const -> size_t
        {
                return sizeof(StrLen) + f_message.size();
        }

        auto Info::message() const -> const std::string &
        {
                return f_message;
        }
} // namespace core::protocol::info
