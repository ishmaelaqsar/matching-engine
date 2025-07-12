#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstring>
#include <netinet/in.h>

#include "types.h"

namespace common
{
        struct MessageHeader
        {
                MessageType type;
                uint32_t length; // payload size (excluding header)

                static constexpr size_t SIZE = sizeof(type) + sizeof(length);

                static void serialize(const MessageHeader message_header, char *data)
                {
                        data[0] = static_cast<uint8_t>(message_header.type); // NOLINT(*-narrowing-conversions)
                        const uint32_t len = htonl(message_header.length); // ensure network byte order
                        std::memcpy(data + 1, &len, sizeof(len));
                }

                static MessageHeader deserialize(const char *data)
                {
                        const auto type = static_cast<MessageType>(data[0]);
                        uint32_t len = 0;
                        std::memcpy(&len, data + 1, sizeof(len));
                        const auto length = ntohl(len);
                        return {type, length};
                }
        };
}

#endif //PROTOCOL_H
