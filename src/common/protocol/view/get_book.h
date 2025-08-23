#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include <utility>

#include "../../types.h"
#include "../serialize_helper.h"
#include "common/protocol/message.h"

namespace common::protocol::view
{
        struct Level
        {
                Price price;
                Quantity quantity;
        };

        class GetBookRequest final : Message
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

                void serialize(unsigned char *data) const override
                {
                        size_t offset = 0;
                        serialize_string(f_symbol, data, &offset);
                }

                void deserialize(unsigned const char *data) override
                {
                        size_t offset = 0;
                        f_symbol = deserialize_string(data, &offset);
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

                friend std::ostream &operator<<(std::ostream &os, const GetBookRequest &request)
                {
                        os << "GetBookRequest{";
                        os << "symbol: " << request.f_symbol;
                        os << "}";
                        return os;
                }

        private:
                Symbol f_symbol{};
        };

        class GetBookResponse final : Message
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

                void serialize(unsigned char *data) const override
                {
                        size_t offset = 0;

                        serialize_uint8(static_cast<uint8_t>(f_bids.size()), data, &offset);
                        for (const auto &bid: f_bids) {
                                serialize_struct(bid, data, &offset);
                        }

                        serialize_uint8(static_cast<uint8_t>(f_asks.size()), data, &offset);
                        for (const auto &ask: f_asks) {
                                serialize_struct(ask, data, &offset);
                        }
                }

                void deserialize(const unsigned char *data) override
                {
                        size_t offset = 0;

                        const size_t num_bids = deserialize_uint8(data, &offset);
                        f_bids = std::vector<Level>(num_bids);
                        for (size_t i = 0; i < num_bids; i++) {
                                f_bids[i] = deserialize_struct<Level>(data, &offset);
                        }

                        const size_t num_asks = deserialize_uint8(data, &offset);
                        f_asks = std::vector<Level>(num_asks);
                        for (size_t i = 0; i < num_asks; i++) {
                                f_asks[i] = deserialize_struct<Level>(data, &offset);
                        }
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

#endif // ADD_ORDER_H
