#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>
#include <utility>
#include <vector>

namespace core::protocol::view
{
    struct Level {
        Level() = default;

        Level(const Price& price, const Quantity& quantity);

        explicit Level(const std::pair<Price, Quantity>& pair);

        std::ostream& operator<<(std::ostream& os) const;

        Price price{};
        Quantity quantity{};
    };

    class GetBookRequest final : public Message<GetBookRequest>
    {
    public:
        GetBookRequest() = default;
        GetBookRequest(const GetBookRequest&) = default;
        GetBookRequest(GetBookRequest&&) = default;
        GetBookRequest& operator=(const GetBookRequest&) = default;
        GetBookRequest& operator=(GetBookRequest&&) = default;
        ~GetBookRequest() = default;

        explicit GetBookRequest(Symbol symbol);

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

    private:
        Symbol f_symbol{};
    };

    class GetBookResponse final : public Message<GetBookResponse>
    {
    public:
        GetBookResponse() = default;
        GetBookResponse(const GetBookResponse& response) = default;
        GetBookResponse(GetBookResponse&& response) = default;
        GetBookResponse& operator=(const GetBookResponse&) = default;
        GetBookResponse& operator=(GetBookResponse&&) = default;
        ~GetBookResponse() = default;

        GetBookResponse(
            Symbol symbol, std::vector<Level> bids, std::vector<Level> asks
        );

        GetBookResponse(
            Symbol symbol, std::initializer_list<Level> bids,
            std::initializer_list<Level> asks
        );

        GetBookResponse(
            Symbol symbol, const std::vector<std::pair<Price, Quantity>>& bids,
            const std::vector<std::pair<Price, Quantity>>& asks
        );

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] Symbol symbol() const;

        [[nodiscard]] std::vector<Level> bids() const;

        [[nodiscard]] std::vector<Level> asks() const;

    private:
        Symbol f_symbol{};
        std::vector<Level> f_bids{};
        std::vector<Level> f_asks{};
    };
} // namespace core::protocol::view
