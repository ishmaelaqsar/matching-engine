#ifndef TYPES_H
#define TYPES_H

#include <boost/lockfree/spsc_queue.hpp>
#include <boost/lockfree/policies.hpp>

namespace common
{
        using Symbol = std::string;
        using ConnectionId = uint32_t;
        using OrderId = uint64_t;
        using TradeId = uint64_t;
        using Timestamp = uint64_t;
        using Price = uint64_t;
        using Quantity = uint64_t;
        using StrLen = uint16_t;

        enum class Side : uint8_t { Buy = 0, Sell = 1 };

        inline std::ostream& operator<<(std::ostream& os, const Side& side)
        {
                switch (side) {
                        case Side::Buy: return os << "Buy";
                        case Side::Sell: return os << "Sell";
                }
                return os;
        }

        enum class MessageType : uint8_t { AddOrderRequest = 1, AddOrderResponse = 2 };

        #ifndef SYMBOL_LEN
        #error "SYMBOL_LEN must be defined at compile time"
        #endif
        constexpr StrLen SymbolLength = SYMBOL_LEN;
        #undef SYMBOL_LEN

        #ifndef RINGBUFFER_CAPACITY
        #define RINGBUFFER_CAPACITY 1024
        #endif
        template<typename T>
        using RingBuffer = boost::lockfree::spsc_queue<T, boost::lockfree::capacity<RINGBUFFER_CAPACITY>>;
        #undef RINGBUFFER_CAPACITY
}

#endif //TYPES_H
