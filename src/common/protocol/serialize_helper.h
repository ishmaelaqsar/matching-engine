#ifndef SERIALIZE_HELPER_H
#define SERIALIZE_HELPER_H

#include <cstring>
#include <limits>
#include <netinet/in.h>
#include <string>

#include "../types.h"

namespace common
{
        static void serialize_string(const std::string &str, char *dst, size_t *offset)
        {
                if (str.length() > std::numeric_limits<StrLen>::max()) {
                        throw std::length_error("String too long to serialize");
                }

                const auto len = static_cast<StrLen>(str.length());
                std::memcpy(dst + *offset, &len, sizeof(len));
                *offset += sizeof(len);

                std::memcpy(dst + *offset, str.data(), str.length());
                *offset += str.length();
        }

        static std::string deserialize_string(const char *src, size_t *offset)
        {
                StrLen length = 0;

                std::memcpy(&length, src + *offset, sizeof(length));
                *offset += sizeof(length);

                auto string = std::string(src + *offset, length);
                *offset += length;

                return string;
        }

        static void serialize_uint64(const uint64_t &uint64, char *dst, size_t *offset)
        {
                const uint64_t ui = htobe64(uint64);
                std::memcpy(dst + *offset, &ui, sizeof(ui));
                *offset += sizeof(ui);
        }

        static uint64_t deserialize_uint64(const char *src, size_t *offset)
        {
                uint64_t ui;
                std::memcpy(&ui, src + *offset, sizeof(ui));
                *offset += sizeof(ui);
                return be64toh(ui);
        }

        static void serialize_uint32(const uint32_t &uint32, char *dst, size_t *offset)
        {
                const uint32_t ui = htobe32(uint32);
                std::memcpy(dst + *offset, &ui, sizeof(ui));
                *offset += sizeof(ui);
        }

        static uint32_t deserialize_uint32(const char *src, size_t *offset)
        {
                uint32_t ui32;
                std::memcpy(&ui32, src + *offset, sizeof(ui32));
                *offset += sizeof(ui32);
                return be32toh(ui32);
        }

        static void serialize_uint16(const uint16_t &uint16, char *dst, size_t *offset)
        {
                const uint16_t ui = htobe16(uint16);
                std::memcpy(dst + *offset, &ui, sizeof(ui));
                *offset += sizeof(ui);
        }

        static uint16_t deserialize_uint16(const char *src, size_t *offset)
        {
                uint16_t ui16;
                std::memcpy(&ui16, src + *offset, sizeof(ui16));
                *offset += sizeof(ui16);
                return be16toh(ui16);
        }

        static void serialize_uint8(const uint8_t &uint8, char *dst, size_t *offset)
        {
                dst[*offset] = static_cast<uint8_t>(uint8); // NOLINT(*-narrowing-conversions)
                *offset += sizeof(uint8);
        }

        static uint8_t deserialize_uint8(const char *src, size_t *offset)
        {
                const auto ui = static_cast<uint8_t>(src[*offset]);
                *offset += sizeof(ui);
                return ui;
        }

        template <typename T>
        static void serialize_struct(const T &t, char *dst, size_t *offset)
        {
                std::memcpy(dst + *offset, &t, sizeof(t));
                *offset += sizeof(t);
        }

        template <typename T>
        static T deserialize_struct(const char *src, size_t *offset)
        {
                T t;
                std::memcpy(&t, src + *offset, sizeof(t));
                *offset += sizeof(t);
                return t;
        }
} // namespace common

#endif // SERIALIZE_HELPER_H
