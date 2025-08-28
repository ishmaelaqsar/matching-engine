#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <boost/lockfree/policies.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include "protocol/header.h"
#include "types.h"

namespace common
{
#ifndef RINGBUFFER_CAPACITY
        #define RINGBUFFER_CAPACITY 1024
#endif
        template<typename T>
        using RingBuffer = boost::lockfree::spsc_queue<T, boost::lockfree::capacity<RINGBUFFER_CAPACITY>>;
#undef RINGBUFFER_CAPACITY

#ifndef MAX_PAYLOAD_SIZE
        #define MAX_PAYLOAD_SIZE 1 << 8
#endif
        constexpr size_t MaxPayloadSize = (MAX_PAYLOAD_SIZE) - sizeof(ConnectionId) - sizeof(protocol::Header::Size);
#undef MAX_PAYLOAD_SIZE

        using Data = std::array<unsigned char, MaxPayloadSize>;

        struct Payload
        {
                ConnectionId connectionId;
                protocol::Header header;
                Data data;
        };
} // namespace common

#endif // RING_BUFFER_H
