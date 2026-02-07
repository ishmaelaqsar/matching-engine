#pragma once

#include <cstring>
#include <limits>
#include <matchingengine/types.h>
#include <stdexcept>
#include <string>

namespace core::protocol
{
    [[maybe_unused]] static void
    serialize_string(const std::string& str, unsigned char* dst, size_t* offset)
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

    [[maybe_unused]] static std::string
    deserialize_string(const unsigned char* src, size_t* offset)
    {
        StrLen length = 0;

        std::memcpy(&length, src + *offset, sizeof(length));
        *offset += sizeof(length);

        auto string =
            std::string(reinterpret_cast<const char*>(src + *offset), length);
        *offset += length;

        return string;
    }

    [[maybe_unused]] static void
    serialize_uint64(const uint64_t& uint64, unsigned char* dst, size_t* offset)
    {
        const auto ui = htobe64(uint64);
        std::memcpy(dst + *offset, &ui, sizeof(ui));
        *offset += sizeof(ui);
    }

    [[maybe_unused]] static uint64_t
    deserialize_uint64(const unsigned char* src, size_t* offset)
    {
        uint64_t ui;
        std::memcpy(&ui, src + *offset, sizeof(ui));
        *offset += sizeof(ui);
        return be64toh(ui);
    }

    [[maybe_unused]] static void
    serialize_uint32(const uint32_t& uint32, unsigned char* dst, size_t* offset)
    {
        const auto ui = htobe32(uint32);
        std::memcpy(dst + *offset, &ui, sizeof(ui));
        *offset += sizeof(ui);
    }

    [[maybe_unused]] static uint32_t
    deserialize_uint32(const unsigned char* src, size_t* offset)
    {
        uint32_t ui32;
        std::memcpy(&ui32, src + *offset, sizeof(ui32));
        *offset += sizeof(ui32);
        return be32toh(ui32);
    }

    [[maybe_unused]] static void
    serialize_uint16(const uint16_t& uint16, unsigned char* dst, size_t* offset)
    {
        const auto ui = htobe16(uint16);
        std::memcpy(dst + *offset, &ui, sizeof(ui));
        *offset += sizeof(ui);
    }

    [[maybe_unused]] static uint16_t
    deserialize_uint16(const unsigned char* src, size_t* offset)
    {
        uint16_t ui16;
        std::memcpy(&ui16, src + *offset, sizeof(ui16));
        *offset += sizeof(ui16);
        return be16toh(ui16);
    }

    [[maybe_unused]] static void
    serialize_uint8(const uint8_t& uint8, unsigned char* dst, size_t* offset)
    {
        dst[*offset] = uint8;
        *offset += sizeof(uint8);
    }

    [[maybe_unused]] static uint8_t
    deserialize_uint8(const unsigned char* src, size_t* offset)
    {
        const auto ui = src[*offset];
        *offset += sizeof(ui);
        return ui;
    }

    template <typename T>
    [[maybe_unused]] static void
    serialize_simple_struct(const T& t, unsigned char* dst, size_t* offset)
    {
        std::memcpy(dst + *offset, &t, sizeof(t));
        *offset += sizeof(t);
    }

    template <typename T>
    [[maybe_unused]] static T
    deserialize_simple_struct(const unsigned char* src, size_t* offset)
    {
        T t;
        std::memcpy(&t, src + *offset, sizeof(t));
        *offset += sizeof(t);
        return t;
    }
} // namespace core::protocol
