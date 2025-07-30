#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "types.h"

constexpr size_t MaxPayloadSize = 256;

namespace common
{
        struct RequestMessage
        {
                ConnectionId connectionId;
                MessageType messageType;
                std::array<char, MaxPayloadSize> payload;
        };

        struct ResponseMessage
        {
                ConnectionId connectionId;
                MessageType messageType;
                std::array<char, MaxPayloadSize> payload;
        };
}

#endif //RING_BUFFER_H
