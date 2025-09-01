#pragma once

#include "core/types.h"
#include "core/protocol/message.h"

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
