#ifndef TYPES_H
#define TYPES_H

#include <cstring>
#include <netinet/in.h>

namespace tcp
{
        using StrLen = uint16_t;

        enum class BookType { Currency, Equity };

        #ifndef BOOK_TYPE
        #error "BOOK_TYPE must be defined at compile time"
        #endif
        #if BOOK_TYPE == Currency
        constexpr auto BOOK_TYPE = BookType::Currency;
        #elif BOOK_TYPE == Equity
        constexpr auto BOOK_TYPE = BookType::Equity;
        #else
        #error "Invalid BOOK_TYPE"
        #endif

        template<BookType T>
        constexpr StrLen max_symbol_len()
        {
                if constexpr (T == BookType::Currency) {
                        return 7;
                } else if constexpr (T == BookType::Equity) {
                        return 4;
                }
                throw std::invalid_argument("Invalid book type");
        }

        enum class Side : uint8_t { Buy = 0, Sell = 1 };

        enum class MessageType : uint8_t { AddOrderRequest = 1, AddOrderResponse = 2 };

        constexpr StrLen MAX_SYMBOL_LEN = max_symbol_len<BOOK_TYPE>();

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
}

#endif //TYPES_H
