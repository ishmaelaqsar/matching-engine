#ifndef MESSAGE_H
#define MESSAGE_H
#include "common/types.h"

namespace common::protocol
{
        class Message
        {
        public:
                virtual ~Message() = default;
                virtual void serialize(unsigned char *dst) const = 0;
                virtual void deserialize(const unsigned char *src) = 0;
                [[nodiscard]] virtual size_t size() const = 0;
                [[nodiscard]] virtual MessageType type() const = 0;
        };
} // namespace common::protocol

#endif // MESSAGE_H
