#ifndef TCP_HEADER_H
#define TCP_HEADER_H

#include <cstring>
#include <netinet/in.h>

#include "../common/types.h"

namespace tcp
{
        struct MessageHeader
        {
                common::MessageType type;
                uint16_t length; // payload size (excluding header)

                static constexpr size_t Size = sizeof(type) + sizeof(length);

                static void serialize(const MessageHeader message_header, char *data)
                {
                        data[0] = static_cast<uint8_t>(message_header.type); // NOLINT(*-narrowing-conversions)
                        const uint16_t len = htons(message_header.length); // ensure network byte order
                        std::memcpy(data + 1, &len, sizeof(len));
                }

                static MessageHeader deserialize(const char *data)
                {
                        const auto type = static_cast<common::MessageType>(data[0]);
                        uint16_t len;
                        std::memcpy(&len, data + 1, sizeof(len));
                        const auto length = ntohs(len);
                        return {type, length};
                }
        };
}

#endif //TCP_HEADER_H
