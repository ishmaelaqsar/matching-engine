#ifndef TCP_HEADER_H
#define TCP_HEADER_H

#include "../common/protocol/serialize_helper.h"
#include "../common/types.h"

namespace tcp
{
        struct MessageHeader
        {
                common::MessageType type;
                size_t length; // payload size (excluding header)

                static constexpr size_t Size = sizeof(type) + sizeof(length);

                static void serialize(const MessageHeader message_header, char *data)
                {
                        if (message_header.length > std::numeric_limits<uint16_t>::max()) {
                                throw std::length_error("header.length too big to serialize");
                        }
                        size_t offset = 0;
                        common::serialize_uint8(static_cast<uint8_t>(message_header.type), data, &offset);
                        common::serialize_uint16(static_cast<uint16_t>(message_header.length), data, &offset);
                }

                static MessageHeader deserialize(const char *data)
                {
                        size_t offset = 0;
                        const auto type = static_cast<common::MessageType>(common::deserialize_uint8(data, &offset));
                        const auto length = common::deserialize_uint16(data, &offset);
                        return {type, length};
                }
        };
} // namespace tcp

#endif // TCP_HEADER_H
