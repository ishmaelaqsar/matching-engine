#pragma once

#include <boost/lockfree/policies.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <matchingengine/protocol/header.h>
#include <matchingengine/types.h>

namespace core
{
    template <typename T>
    using RingBuffer = boost::lockfree::spsc_queue<
        T, boost::lockfree::capacity<RINGBUFFER_CAPACITY>>;

    static constexpr auto MaxPayloadSize = MAX_PAYLOAD_SIZE -
                                           sizeof(ConnectionId) -
                                           sizeof(protocol::Header::Size);

    using Data = std::array<unsigned char, MaxPayloadSize>;

    struct Payload {
        ConnectionId connection_id;
        protocol::Header header;
        Data data;
    };
} // namespace core
