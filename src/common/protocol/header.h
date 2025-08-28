#ifndef HEADER_H
#define HEADER_H

#include "../types.h"
#include "serialize_helper.h"

namespace common::protocol
{
        struct Header
        {
                MessageType type;
                size_t length; // payload size (excluding header)

                static constexpr size_t Size = sizeof(type) + sizeof(length);

                static void serialize(const Header message_header, unsigned char *data)
                {
                        if (message_header.length > std::numeric_limits<size_t>::max()) {
                                throw std::length_error("header.length too big to serialize");
                        }
                        size_t offset = 0;
                        serialize_uint8(static_cast<uint8_t>(message_header.type), data, &offset);
                        serialize_uint16(static_cast<uint16_t>(message_header.length), data, &offset);
                }

                static Header deserialize(const unsigned char *data)
                {
                        size_t offset = 0;
                        const auto type = static_cast<MessageType>(deserialize_uint8(data, &offset));
                        const auto length = deserialize_uint16(data, &offset);
                        return {type, length};
                }
        };
} // namespace tcp

#endif // HEADER_H
