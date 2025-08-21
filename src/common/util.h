#ifndef UTIL_H
#define UTIL_H

#include <cstring>
#include <limits>
#include <string>
#include <netinet/in.h>

#include "types.h"

namespace common
{
        // Serialize a variable length string
        static size_t serialize_string(const std::string &str, char *data, const size_t offset)
        {
                if (str.length() > std::numeric_limits<StrLen>::max()) {
                        throw std::length_error("String too long to serialize");
                }
                const StrLen len = htons(static_cast<StrLen>(str.length())); // network byte order
                std::memcpy(data + offset, &len, sizeof(len));
                std::memcpy(data + offset + sizeof(len), str.data(), str.length());
                return sizeof(len) + str.length();
        }

        // Deserialize a variable length string
        static std::string deserialize_string(const char *src, size_t *offset)
        {
                StrLen len = 0;
                std::memcpy(&len, src + *offset, sizeof(len));
                *offset += sizeof(len);
                const auto length = ntohs(len);

                auto string = std::string(src + *offset, length);
                *offset += length;
                return string;
        }
}

#endif //UTIL_H
