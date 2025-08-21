#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <boost/lockfree/policies.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include "../types.h"

namespace common
{
#ifndef RINGBUFFER_CAPACITY
        #define RINGBUFFER_CAPACITY 1024
#endif
        template<typename T>
        using RingBuffer = boost::lockfree::spsc_queue<T, boost::lockfree::capacity<RINGBUFFER_CAPACITY>>;
#undef RINGBUFFER_CAPACITY

#ifndef MAX_PAYLOAD_SIZE
        #define MAX_PAYLOAD_SIZE 256
#endif
        constexpr size_t MaxPayloadSize = MAX_PAYLOAD_SIZE;
#undef MAX_PAYLOAD_SIZE

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
} // namespace common

#endif // RING_BUFFER_H
