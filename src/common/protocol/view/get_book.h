#ifndef ADD_ORDER_H
#define ADD_ORDER_H

#include <cstring>
#include <utility>

#include "../../types.h"

namespace common::protocol::view
{
        struct Level
        {
                uint64_t price;
                uint64_t quantity;
        };

        class GetBookRequest
        {
        public:
                static void serialize(const GetBookRequest &request, char *data)
                {
                        size_t offset = 0;

                        const uint8_t s = request.f_symbol.length();
                        std::memcpy(data + offset, &s, sizeof(s));
                        offset += sizeof(s);

                        std::memcpy(data + offset, request.f_symbol.data(), s);
                }

                static GetBookRequest deserialize(const char *data)
                {
                        size_t offset = 0;

                        uint8_t s;
                        std::memcpy(&s, data + offset, sizeof(s));
                        offset += sizeof(s);

                        Symbol symbol;
                        symbol.resize(s);
                        std::memcpy(symbol.data(), data + offset, s);

                        return GetBookRequest{symbol};
                }

                GetBookRequest() = default;
                GetBookRequest(const GetBookRequest &) = default;
                GetBookRequest(GetBookRequest &&) = default;
                GetBookRequest &operator=(const GetBookRequest &) = default;
                GetBookRequest &operator=(GetBookRequest &&) = default;
                ~GetBookRequest() = default;

                explicit GetBookRequest(Symbol symbol) : f_symbol(std::move(symbol))
                {}

                [[nodiscard]] uint16_t size() const
                {
                        return f_symbol.size();
                }

                [[nodiscard]] Symbol symbol() const
                {
                        return f_symbol;
                }

        private:
                Symbol f_symbol{};
        };

        class GetBookResponse
        {
        public:
                static void serialize(const GetBookResponse &response, char *data)
                {
                        size_t offset = 0;
                        const uint8_t num_bids = response.f_bids.size();
                        const uint8_t num_asks = response.f_asks.size();

                        std::memcpy(data + offset, &num_bids, sizeof(num_bids));
                        offset += sizeof(num_bids);

                        for (auto i = 0; i < num_bids; i++) {
                                std::memcpy(data + offset, &response.f_bids[i], sizeof(response.f_bids[i]));
                                offset += sizeof(response.f_bids[i]);
                        }

                        std::memcpy(data + offset, &num_asks, sizeof(num_asks));
                        offset += sizeof(num_asks);

                        for (auto i = 0; i < num_asks; i++) {
                                std::memcpy(data + offset, &response.f_asks[i], sizeof(response.f_asks[i]));
                                offset += sizeof(response.f_asks[i]);
                        }
                }

                static GetBookResponse deserialize(const char *data)
                {
                        size_t offset = 0;

                        uint8_t num_bids;
                        std::memcpy(&num_bids, data + offset, sizeof(num_bids));
                        offset += sizeof(num_bids);

                        auto bids = std::vector<Level>(num_bids);
                        for (auto i = 0; i < num_bids; i++) {
                                std::memcpy(bids.data() + i, &data[offset], sizeof(bids[i]));
                                offset += sizeof(bids[i]);
                        }

                        uint8_t num_asks;
                        std::memcpy(&num_asks, data + offset, sizeof(num_asks));
                        offset += sizeof(num_asks);

                        auto asks = std::vector<Level>(num_asks);
                        for (auto i = 0; i < num_asks; i++) {
                                std::memcpy(asks.data() + i, &data[offset], sizeof(asks[i]));
                                offset += sizeof(asks[i]);
                        }

                        return {std::move(bids), std::move(asks)};
                }

                GetBookResponse() = default;
                GetBookResponse(const GetBookResponse &response) = default;
                GetBookResponse(GetBookResponse &&response) = default;
                GetBookResponse &operator=(const GetBookResponse &) = default;
                GetBookResponse &operator=(GetBookResponse &&) = default;
                ~GetBookResponse() = default;

                GetBookResponse(std::vector<Level> bids, std::vector<Level> asks) :
                    f_bids(std::move(bids)), f_asks(std::move(asks))
                {}

                GetBookResponse(const std::initializer_list<Level> bids, const std::initializer_list<Level> asks) :
                    f_bids(bids), f_asks(asks)
                {}

                [[nodiscard]] uint16_t size() const
                {
                        return sizeof(f_bids.size()) + (f_bids.size() * sizeof(Level)) + sizeof(f_asks.size()) +
                               (f_asks.size() * sizeof(Level));
                }

                [[nodiscard]] std::vector<Level> get_bids() const
                {
                        return f_bids;
                }

                [[nodiscard]] std::vector<Level> get_asks() const
                {
                        return f_asks;
                }

        private:
                std::vector<Level> f_bids{};
                std::vector<Level> f_asks{};
        };
} // namespace common::protocol::view

#endif // ADD_ORDER_H
