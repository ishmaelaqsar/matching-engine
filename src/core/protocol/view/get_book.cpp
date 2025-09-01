#include "core/protocol/view/get_book.h"
#include "core/protocol/serialize_helper.h"

namespace core::protocol::view
{

        Level::Level(const Price &price, const Quantity &quantity) : price(price), quantity(quantity)
        {}

        Level::Level(const std::pair<Price, Quantity> &pair) : price(std::get<0>(pair)), quantity(std::get<1>(pair))
        {}

        std::ostream &Level::operator<<(std::ostream &os) const
        {
                os << "Level{";
                os << "price: " << price << ", quantity: " << quantity;
                os << "}";
                return os;
        }

        GetBookRequest::GetBookRequest(Symbol symbol) : f_symbol(std::move(symbol))
        {}

        void GetBookRequest::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
        }

        void GetBookRequest::deserialize(unsigned const char *src)
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
        }

        void GetBookRequest::print(std::ostream &os) const
        {
                os << "GetBookRequest{";
                os << "symbol: " << f_symbol;
                os << "}";
        }

        GetBookResponse::GetBookResponse(std::vector<Level> bids, std::vector<Level> asks) :
            f_bids(std::move(bids)), f_asks(std::move(asks))
        {}

        GetBookResponse::GetBookResponse(const std::initializer_list<Level> bids,
                                         const std::initializer_list<Level> asks) :
            f_bids(bids), f_asks(asks)
        {}

        GetBookResponse::GetBookResponse(std::vector<std::pair<Price, Quantity>> &&bids,
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

        void GetBookResponse::serialize(unsigned char *dst) const
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

        void GetBookResponse::deserialize(const unsigned char *src)
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

        void GetBookResponse::print(std::ostream &os) const
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
} // namespace core::protocol::view
