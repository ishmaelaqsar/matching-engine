#include <matchingengine/protocol/view/get_book.h>

#include <sstream>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol::view
{

        Level::Level(const Price &price, const Quantity &quantity) : price(price), quantity(quantity)
        {}

        Level::Level(const std::pair<Price, Quantity> &pair) : price(std::get<0>(pair)), quantity(std::get<1>(pair))
        {}

        auto Level::operator<<(std::ostream &os) const -> std::ostream &
        {
                os << "Level{";
                os << "price: " << price << ", quantity: " << quantity;
                os << "}";
                return os;
        }

        GetBookRequest::GetBookRequest(Symbol symbol) : f_symbol(std::move(symbol))
        {}

        auto GetBookRequest::type() -> MessageType
        {
                return MessageType::GetBookRequest;
        }

        auto GetBookRequest::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
        }

        auto GetBookRequest::deserialize(unsigned const char *src) -> void
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
        }

        auto GetBookRequest::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "GetBookRequest{";
                os << "symbol: " << f_symbol;
                os << "}";
                return os.str();
        }

        auto GetBookRequest::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto GetBookRequest::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.size();
        }

        auto GetBookRequest::symbol() const -> Symbol
        {
                return f_symbol;
        }

        GetBookResponse::GetBookResponse(Symbol symbol, std::vector<Level> bids, std::vector<Level> asks) :
            f_symbol(std::move(symbol)), f_bids(std::move(bids)), f_asks(std::move(asks))
        {}

        GetBookResponse::GetBookResponse(Symbol symbol, const std::initializer_list<Level> bids,
                                         const std::initializer_list<Level> asks) :
            f_symbol(std::move(symbol)), f_bids(bids), f_asks(asks)
        {}

        GetBookResponse::GetBookResponse(Symbol symbol, const std::vector<std::pair<Price, Quantity>> &bids,
                                         const std::vector<std::pair<Price, Quantity>> &asks) :
            f_symbol(std::move(symbol))
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

        auto GetBookResponse::type() -> MessageType
        {
                return MessageType::GetBookResponse;
        }

        auto GetBookResponse::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;

                serialize_string(f_symbol, dst, &offset);

                serialize_uint8(static_cast<uint8_t>(f_bids.size()), dst, &offset);
                for (const auto &bid: f_bids) {
                        serialize_simple_struct(bid, dst, &offset);
                }

                serialize_uint8(static_cast<uint8_t>(f_asks.size()), dst, &offset);
                for (const auto &ask: f_asks) {
                        serialize_simple_struct(ask, dst, &offset);
                }
        }

        auto GetBookResponse::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;

                f_symbol = deserialize_string(src, &offset);

                const size_t num_bids = deserialize_uint8(src, &offset);
                f_bids = std::vector<Level>(num_bids);
                for (size_t i = 0; i < num_bids; i++) {
                        f_bids[i] = deserialize_simple_struct<Level>(src, &offset);
                }

                const size_t num_asks = deserialize_uint8(src, &offset);
                f_asks = std::vector<Level>(num_asks);
                for (size_t i = 0; i < num_asks; i++) {
                        f_asks[i] = deserialize_simple_struct<Level>(src, &offset);
                }
        }
        auto GetBookResponse::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "GetBookResponse{";
                os << "symbol: " << f_symbol << ", ";
                os << "bids: [";
                for (const auto bid: f_bids) {
                        bid.operator<<(os) << ", ";
                }
                os << "], asks: [";
                for (const auto ask: f_asks) {
                        ask.operator<<(os) << ", ";
                }
                os << "]}";
                return os.str();
        }

        auto GetBookResponse::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto GetBookResponse::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.size() + 1 + (f_bids.size() * sizeof(Level)) + 1 +
                       (f_asks.size() * sizeof(Level));
        }

        auto GetBookResponse::symbol() const -> Symbol
        {
                return f_symbol;
        }

        auto GetBookResponse::bids() const -> std::vector<Level>
        {
                return f_bids;
        }

        auto GetBookResponse::asks() const -> std::vector<Level>
        {
                return f_asks;
        }
} // namespace core::protocol::view
