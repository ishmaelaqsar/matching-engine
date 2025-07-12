#ifndef ENGINE_H
#define ENGINE_H

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/policies.hpp>

namespace engine
{
        #ifndef RINGBUFFER_CAPACITY
        #define RINGBUFFER_CAPACITY 1024
        #endif

        template<typename T>
        using RingBuffer = boost::lockfree::spsc_queue<T, boost::lockfree::capacity<RINGBUFFER_CAPACITY>>;
}

#endif //ENGINE_H
