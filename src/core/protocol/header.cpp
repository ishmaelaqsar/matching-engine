#include <core/protocol/header.h>
#include <core/protocol/serialize_helper.h>

namespace core::protocol
{
        void Header::serialize(const Header message_header, unsigned char *dst)
        {
                size_t offset = 0;
                serialize_uint8(static_cast<uint8_t>(message_header.type), dst, &offset);
                serialize_uint16(static_cast<uint16_t>(message_header.length), dst, &offset);
        }

        Header Header::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                const auto type = static_cast<MessageType>(deserialize_uint8(src, &offset));
                const auto length = deserialize_uint16(src, &offset);
                return {type, length};
        }
} // namespace core::protocol
