#include <matchingengine/protocol/header.h>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol
{
        auto Header::serialize(const Header message_header, unsigned char *dst) -> void
        {
                size_t offset = 0;
                serialize_uint8(static_cast<uint8_t>(message_header.type), dst, &offset);
                serialize_uint16(static_cast<uint16_t>(message_header.length), dst, &offset);
        }

        auto Header::deserialize(const unsigned char *src) -> Header
        {
                size_t offset = 0;
                const auto type = static_cast<MessageType>(deserialize_uint8(src, &offset));
                const auto length = deserialize_uint16(src, &offset);
                return {type, length};
        }
} // namespace core::protocol
