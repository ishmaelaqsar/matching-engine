#ifndef PROTOCOL_GET_BOOK_H
#define PROTOCOL_GET_BOOK_H

#include <utility>

#include "../../types.h"
#include "../serialize_helper.h"
#include "common/protocol/message.h"

namespace common::protocol::view
{
        class Level
        {
        public:
                Level() = default;

                Level(const Price &price, const Quantity &quantity) : price(price), quantity(quantity)
                {}

                explicit Level(const std::pair<Price, Quantity> &pair) :
                    price(std::get<0>(pair)), quantity(std::get<1>(pair))
                {}

                std::ostream &operator<<(std::ostream &os) const
                {
                        os << "Level{";
                        os << "price: " << price << ", quantity: " << quantity;
                        os << "}";
                        return os;
                }

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

                explicit GetBookRequest(Symbol symbol) : f_symbol(std::move(symbol))
                {}

                void serialize(unsigned char *dst) const override
                {
                        size_t offset = 0;
                        serialize_string(f_symbol, dst, &offset);
                }

                void deserialize(unsigned const char *src) override
                {
                        size_t offset = 0;
                        f_symbol = deserialize_string(src, &offset);
                }

                void print(std::ostream &os) const override
                {
                        os << "GetBookRequest{";
                        os << "symbol: " << f_symbol;
                        os << "}";
                }

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

                GetBookResponse(std::vector<Level> bids, std::vector<Level> asks) :
                    f_bids(std::move(bids)), f_asks(std::move(asks))
                {}

                GetBookResponse(const std::initializer_list<Level> bids, const std::initializer_list<Level> asks) :
                    f_bids(bids), f_asks(asks)
                {}

                GetBookResponse(std::vector<std::pair<Price, Quantity>> &&bids,
                                std::vector<std::pair<Price, Quantity>> &&asks)
                {
                        f_bids.reserve(bids.size());
                        for (auto bid: bids) {
                                f_bids.emplace_back(std::move(bid));
                        }

                        f_asks.reserve(asks.size());
                        for (auto ask: asks) {
                                f_asks.emplace_back(std::move(ask));
                        }
                }

                void serialize(unsigned char *dst) const override
                {
                        size_t offset = 0;

                        serialize_uint8(static_cast<uint8_t>(f_bids.size()), dst, &offset);
                        for (const auto &bid: f_bids) {
                                serialize_struct(bid, dst, &offset);
                        }

                        serialize_uint8(static_cast<uint8_t>(f_asks.size()), dst, &offset);
                        for (const auto &ask: f_asks) {
                                serialize_struct(ask, dst, &offset);
                        }
                }

                void deserialize(const unsigned char *src) override
                {
                        size_t offset = 0;

                        const size_t num_bids = deserialize_uint8(src, &offset);
                        f_bids = std::vector<Level>(num_bids);
                        for (size_t i = 0; i < num_bids; i++) {
                                f_bids[i] = deserialize_struct<Level>(src, &offset);
                        }

                        const size_t num_asks = deserialize_uint8(src, &offset);
                        f_asks = std::vector<Level>(num_asks);
                        for (size_t i = 0; i < num_asks; i++) {
                                f_asks[i] = deserialize_struct<Level>(src, &offset);
                        }
                }

                void print(std::ostream &os) const override
                {
                        os << "GetBookResponse{";
                        os << "bids: [";
                        for (const auto bid: f_bids) {
                                bid.operator<<(os) << ", ";
                        }
                        os << "], asks: [";
                        for (const auto ask: f_asks) {
                                ask.operator<<(os) << ", ";
                        }
                        os << "]}";
                }

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
} // namespace common::protocol::view

#endif // PROTOCOL_GET_BOOK_H
