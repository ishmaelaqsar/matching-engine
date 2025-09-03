#pragma once

#include <core/protocol/message.h>
#include <core/types.h>

namespace core::protocol
{
        struct Header
        {
                MessageType type;
                size_t length; // payload size (excluding header)

                static constexpr size_t Size = sizeof(type) + sizeof(length);

                static void serialize(Header message_header, unsigned char *dst);

                static Header deserialize(const unsigned char *src);
        };
} // namespace core::protocol
