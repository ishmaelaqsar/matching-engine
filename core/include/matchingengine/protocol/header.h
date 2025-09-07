#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol
{
        struct Header
        {
                MessageType type;
                size_t length; // payload size (excluding header)

                static constexpr auto Size = sizeof(type) + sizeof(length);

                static auto serialize(Header message_header, unsigned char *dst) -> void;

                static auto deserialize(const unsigned char *src) -> Header;
        };
} // namespace core::protocol
