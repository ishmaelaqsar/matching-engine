#pragma once

#include <utility>
#include <vector>

#include <core/protocol/message.h>
#include <core/types.h>

namespace core::protocol::view
{
        class Level
        {
        public:
                Level() = default;

                Level(const Price &price, const Quantity &quantity);

                explicit Level(const std::pair<Price, Quantity> &pair);

                std::ostream &operator<<(std::ostream &os) const;

                Price price;
                Quantity quantity;
        };

        class GetBookRequest final : public Message
        {
        public:
                GetBookRequest() = default;
                GetBookRequest(const GetBookRequest &) = default;
                GetBookRequest(GetBookRequest &&) = default;
                GetBookRequest &operator=(const GetBookRequest &) = default;
                GetBookRequest &operator=(GetBookRequest &&) = default;
                ~GetBookRequest() override = default;

                explicit GetBookRequest(Symbol symbol);

                void serialize(unsigned char *dst) const override;

                void deserialize(unsigned const char *src) override;

                void print(std::ostream &os) const override;

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(StrLen) + f_symbol.size();
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::GetBookRequest;
                }

                [[nodiscard]] Symbol symbol() const
                {
                        return f_symbol;
                }

        private:
                Symbol f_symbol{};
        };

        class GetBookResponse final : public Message
        {
        public:
                GetBookResponse() = default;
                GetBookResponse(const GetBookResponse &response) = default;
                GetBookResponse(GetBookResponse &&response) = default;
                GetBookResponse &operator=(const GetBookResponse &) = default;
                GetBookResponse &operator=(GetBookResponse &&) = default;
                ~GetBookResponse() override = default;

                GetBookResponse(std::vector<Level> bids, std::vector<Level> asks);

                GetBookResponse(std::initializer_list<Level> bids, std::initializer_list<Level> asks);

                GetBookResponse(std::vector<std::pair<Price, Quantity>> &&bids,
                                std::vector<std::pair<Price, Quantity>> &&asks);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

                [[nodiscard]] size_t size() const override
                {
                        return 1 + (f_bids.size() * sizeof(Level)) + 1 + (f_asks.size() * sizeof(Level));
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::GetBookResponse;
                }

                [[nodiscard]] std::vector<Level> bids() const
                {
                        return f_bids;
                }

                [[nodiscard]] std::vector<Level> asks() const
                {
                        return f_asks;
                }

        private:
                std::vector<Level> f_bids{};
                std::vector<Level> f_asks{};
        };
} // namespace core::protocol::view
