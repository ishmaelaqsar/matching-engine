#pragma once

#include <cstring>
#include <limits>
#include <netinet/in.h>
#include <stdexcept>
#include <string>

#include <matchingengine/types.h>

namespace core::protocol
{
        static auto serialize_string(const std::string &str, unsigned char *dst, size_t *offset) -> void
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

        static auto deserialize_string(const unsigned char *src, size_t *offset) -> std::string
        {
                StrLen length = 0;

                std::memcpy(&length, src + *offset, sizeof(length));
                *offset += sizeof(length);

                auto string = std::string(reinterpret_cast<const char *>(src + *offset), length);
                *offset += length;

                return string;
        }

        static auto serialize_uint64(const uint64_t &uint64, unsigned char *dst, size_t *offset) -> void
        {
                const uint64_t ui = htobe64(uint64);
                std::memcpy(dst + *offset, &ui, sizeof(ui));
                *offset += sizeof(ui);
        }

        static auto deserialize_uint64(const unsigned char *src, size_t *offset) -> uint64_t
        {
                uint64_t ui;
                std::memcpy(&ui, src + *offset, sizeof(ui));
                *offset += sizeof(ui);
                return be64toh(ui);
        }

        static auto serialize_uint32(const uint32_t &uint32, unsigned char *dst, size_t *offset) -> void
        {
                const uint32_t ui = htobe32(uint32);
                std::memcpy(dst + *offset, &ui, sizeof(ui));
                *offset += sizeof(ui);
        }

        static auto deserialize_uint32(const unsigned char *src, size_t *offset) -> uint32_t
        {
                uint32_t ui32;
                std::memcpy(&ui32, src + *offset, sizeof(ui32));
                *offset += sizeof(ui32);
                return be32toh(ui32);
        }

        static auto serialize_uint16(const uint16_t &uint16, unsigned char *dst, size_t *offset) -> void
        {
                const uint16_t ui = htobe16(uint16);
                std::memcpy(dst + *offset, &ui, sizeof(ui));
                *offset += sizeof(ui);
        }

        static auto deserialize_uint16(const unsigned char *src, size_t *offset) -> uint16_t
        {
                uint16_t ui16;
                std::memcpy(&ui16, src + *offset, sizeof(ui16));
                *offset += sizeof(ui16);
                return be16toh(ui16);
        }

        static auto serialize_uint8(const uint8_t &uint8, unsigned char *dst, size_t *offset) -> void
        {
                dst[*offset] = uint8;
                *offset += sizeof(uint8);
        }

        static auto deserialize_uint8(const unsigned char *src, size_t *offset) -> uint8_t
        {
                const auto ui = src[*offset];
                *offset += sizeof(ui);
                return ui;
        }

        template <typename T>
        static auto serialize_simple_struct(const T &t, unsigned char *dst, size_t *offset) -> void
        {
                std::memcpy(dst + *offset, &t, sizeof(t));
                *offset += sizeof(t);
        }

        template <typename T>
        static auto deserialize_simple_struct(const unsigned char *src, size_t *offset) -> T
        {
                T t;
                std::memcpy(&t, src + *offset, sizeof(t));
                *offset += sizeof(t);
                return t;
        }
} // namespace core::protocol
