#pragma once

#include <utility>
#include <vector>

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::view
{
        class Level
        {
        public:
                Level() = default;

                Level(const Price &price, const Quantity &quantity);

                explicit Level(const std::pair<Price, Quantity> &pair);

                auto operator<<(std::ostream &os) const -> std::ostream &;

                Price price;
                Quantity quantity;
        };

        class GetBookRequest final : public Message<GetBookRequest>
        {
        public:
                GetBookRequest() = default;
                GetBookRequest(const GetBookRequest &) = default;
                GetBookRequest(GetBookRequest &&) = default;
                GetBookRequest &operator=(const GetBookRequest &) = default;
                GetBookRequest &operator=(GetBookRequest &&) = default;
                ~GetBookRequest() = default;

                explicit GetBookRequest(Symbol symbol);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(unsigned const char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto symbol() const -> Symbol;

        private:
                Symbol f_symbol{};
        };

        class GetBookResponse final : public Message<GetBookResponse>
        {
        public:
                GetBookResponse() = default;
                GetBookResponse(const GetBookResponse &response) = default;
                GetBookResponse(GetBookResponse &&response) = default;
                GetBookResponse &operator=(const GetBookResponse &) = default;
                GetBookResponse &operator=(GetBookResponse &&) = default;
                ~GetBookResponse() = default;

                GetBookResponse(Symbol symbol, std::vector<Level> bids, std::vector<Level> asks);

                GetBookResponse(Symbol symbol, std::initializer_list<Level> bids, std::initializer_list<Level> asks);

                GetBookResponse(Symbol symbol, const std::vector<std::pair<Price, Quantity>> &bids,
                                const std::vector<std::pair<Price, Quantity>> &asks);

                static auto type() -> MessageType;

                auto serialize(unsigned char *dst) const -> void;

                auto deserialize(const unsigned char *src) -> void;

                auto print(std::ostream &os) const -> void;

                [[nodiscard]] auto to_string() const -> std::string;

                [[nodiscard]] auto size() const -> size_t;

                [[nodiscard]] auto symbol() const -> Symbol;

                [[nodiscard]] auto bids() const -> std::vector<Level>;

                [[nodiscard]] auto asks() const -> std::vector<Level>;

        private:
                Symbol f_symbol{};
                std::vector<Level> f_bids{};
                std::vector<Level> f_asks{};
        };
} // namespace core::protocol::view
