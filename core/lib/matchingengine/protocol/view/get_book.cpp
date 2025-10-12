#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/protocol/view/get_book.h>
#include <sstream>

namespace core::protocol::view
{

    Level::Level(const Price& price, const Quantity& quantity)
        : price(price), quantity(quantity)
    {
    }

    Level::Level(const std::pair<Price, Quantity>& pair)
        : price(std::get<0>(pair)), quantity(std::get<1>(pair))
    {
    }

    std::ostream& Level::operator<<(std::ostream& os) const
    {
        os << "Level{";
        os << "price: " << price << ", quantity: " << quantity;
        os << "}";
        return os;
    }

    GetBookRequest::GetBookRequest(Symbol symbol) : f_symbol(std::move(symbol))
    {
    }

    MessageType GetBookRequest::type()
    {
        return MessageType::GetBookRequest;
    }

    void GetBookRequest::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_symbol, dst, &offset);
    }

    void GetBookRequest::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_symbol = deserialize_string(src, &offset);
    }

    void GetBookRequest::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string GetBookRequest::to_string() const
    {
        std::ostringstream os;
        os << "GetBookRequest{";
        os << "symbol: " << f_symbol;
        os << "}";
        return os.str();
    }

    size_t GetBookRequest::size() const
    {
        return sizeof(StrLen) + f_symbol.size();
    }

    Symbol GetBookRequest::symbol() const
    {
        return f_symbol;
    }

    GetBookResponse::GetBookResponse(
        Symbol symbol, std::vector<Level> bids, std::vector<Level> asks
    )
        : f_symbol(std::move(symbol)), f_bids(std::move(bids)),
          f_asks(std::move(asks))
    {
    }

    GetBookResponse::GetBookResponse(
        Symbol symbol, const std::initializer_list<Level> bids,
        const std::initializer_list<Level> asks
    )
        : f_symbol(std::move(symbol)), f_bids(bids), f_asks(asks)
    {
    }

    GetBookResponse::GetBookResponse(
        Symbol symbol, const std::vector<std::pair<Price, Quantity>>& bids,
        const std::vector<std::pair<Price, Quantity>>& asks
    )
        : f_symbol(std::move(symbol))
    {
        f_bids.reserve(bids.size());
        for (auto bid : bids) {
            f_bids.emplace_back(std::move(bid));
        }

        f_asks.reserve(asks.size());
        for (auto ask : asks) {
            f_asks.emplace_back(std::move(ask));
        }
    }

    MessageType GetBookResponse::type()
    {
        return MessageType::GetBookResponse;
    }

    void GetBookResponse::serialize(unsigned char* dst) const
    {
        size_t offset = 0;

        serialize_string(f_symbol, dst, &offset);

        serialize_uint8(static_cast<uint8_t>(f_bids.size()), dst, &offset);
        for (const auto& bid : f_bids) {
            serialize_simple_struct(bid, dst, &offset);
        }

        serialize_uint8(static_cast<uint8_t>(f_asks.size()), dst, &offset);
        for (const auto& ask : f_asks) {
            serialize_simple_struct(ask, dst, &offset);
        }
    }

    void GetBookResponse::deserialize(const unsigned char* src)
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

    void GetBookResponse::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string GetBookResponse::to_string() const
    {
        std::ostringstream os;
        os << "GetBookResponse{";
        os << "symbol: " << f_symbol << ", ";
        os << "bids: [";
        for (const auto& bid : f_bids) {
            bid.operator<<(os) << ", ";
        }
        os << "], asks: [";
        for (const auto& ask : f_asks) {
            ask.operator<<(os) << ", ";
        }
        os << "]}";
        return os.str();
    }

    size_t GetBookResponse::size() const
    {
        return sizeof(StrLen) + f_symbol.size() + 1 +
               (f_bids.size() * sizeof(Level)) + 1 +
               (f_asks.size() * sizeof(Level));
    }

    Symbol GetBookResponse::symbol() const
    {
        return f_symbol;
    }

    std::vector<Level> GetBookResponse::bids() const
    {
        return f_bids;
    }

    std::vector<Level> GetBookResponse::asks() const
    {
        return f_asks;
    }
} // namespace core::protocol::view
