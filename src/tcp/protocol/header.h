#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstring>
#include <netinet/in.h>

namespace orderbook
{
        using StrLen = uint16_t;

        enum class Side : uint8_t { Buy = 0, Sell = 1 };

        enum class MessageType : uint8_t { AddOrderRequest = 1, AddOrderResponse = 2 };

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

        // Serialize a fixed length string
        static size_t serialize_string(const std::string &str, char *data, const size_t offset, const StrLen length)
        {
                char buffer[length];
                const StrLen len = std::min(str.length(), static_cast<size_t>(length));
                std::memcpy(buffer, str.data(), len);
                std::memcpy(data + offset, buffer, length);
                return length;
        }

        // Deserialize a variable length string
        static std::string deserialize_string(const char *data, size_t *offset)
        {
                StrLen len = 0;
                std::memcpy(&len, data + *offset, sizeof(len));
                *offset += sizeof(len);
                const auto length = ntohs(len);

                const auto string = std::string(data + *offset, length);
                *offset += length;
                return string;
        }

        // Deserialize a fixed length string
        static std::string deserialize_string(const char *data, size_t *offset, const StrLen length)
        {
                const auto string = std::string(data + *offset, length);
                *offset += length;
                return string;
        }

        struct MessageHeader
        {
                MessageType type;
                uint32_t length; // payload size (excluding header)

                static constexpr size_t SIZE = sizeof(type) + sizeof(length);

                static void serialize(const MessageHeader message_header, char *data)
                {
                        data[0] = static_cast<uint8_t>(message_header.type); // NOLINT(*-narrowing-conversions)
                        const uint32_t len = htonl(message_header.length); // ensure network byte order
                        std::memcpy(data + 1, &len, sizeof(len));
                }

                static MessageHeader deserialize(const char *data)
                {
                        const auto type = static_cast<MessageType>(data[0]);
                        uint32_t len = 0;
                        std::memcpy(&len, data + 1, sizeof(len));
                        const auto length = ntohl(len);
                        return {type, length};
                }
        };
}

#endif //PROTOCOL_H
