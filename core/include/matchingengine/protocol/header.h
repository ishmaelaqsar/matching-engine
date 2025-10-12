#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol
{
    struct Header {
        MessageType type;
        size_t length; // payload size (excluding header)

        static constexpr auto Size = sizeof(type) + sizeof(length);

        static void serialize(Header message_header, unsigned char* dst);

        static Header deserialize(const unsigned char* src);
    };
} // namespace core::protocol
