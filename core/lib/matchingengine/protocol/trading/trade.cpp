#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/protocol/trading/trade.h>
#include <sstream>

namespace core::protocol::trading
{
    Trade::Trade(
        Symbol symbol, const TradeId id, const Price price,
        const Quantity quantity, const Timestamp timestamp,
        const OrderId source_order, const OrderId matched_order
    )
        : f_symbol(std::move(symbol)), f_id(id), f_price(price),
          f_quantity(quantity), f_timestamp(timestamp),
          f_source_order(source_order), f_matched_order(matched_order)
    {
    }

    MessageType Trade::type()
    {
        return MessageType::Trade;
    }

    void Trade::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_uint64(f_id, dst, &offset);
        serialize_uint64(f_price, dst, &offset);
        serialize_uint64(f_quantity, dst, &offset);
        serialize_uint64(f_timestamp, dst, &offset);
        serialize_uint64(f_source_order, dst, &offset);
        serialize_uint64(f_matched_order, dst, &offset);
    }

    void Trade::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_id = deserialize_uint64(src, &offset);
        f_price = deserialize_uint64(src, &offset);
        f_quantity = deserialize_uint64(src, &offset);
        f_timestamp = deserialize_uint64(src, &offset);
        f_source_order = deserialize_uint64(src, &offset);
        f_matched_order = deserialize_uint64(src, &offset);
    }

    void Trade::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string Trade::to_string() const
    {
        std::ostringstream os;
        os << "Trade{";
        os << "id: " << f_id << ", ";
        os << "price: " << f_price << ", ";
        os << "quantity: " << f_quantity << ", ";
        os << "timestamp: " << f_timestamp << ", ";
        os << "source_order: " << f_source_order << ", ";
        os << "matched_order: " << f_matched_order;
        os << "}";
        return os.str();
    }

    size_t Trade::size() const
    {
        return sizeof(StrLen) + f_symbol.length() + sizeof(f_id) +
               sizeof(f_price) + sizeof(f_quantity) + sizeof(f_timestamp) +
               sizeof(f_source_order) + sizeof(f_matched_order);
    }

    const Symbol& Trade::symbol() const
    {
        return f_symbol;
    }

    TradeId Trade::id() const
    {
        return f_id;
    }

    Price Trade::price() const
    {
        return f_price;
    }

    Quantity Trade::quantity() const
    {
        return f_quantity;
    }

    Timestamp Trade::timestamp() const
    {
        return f_timestamp;
    }

    OrderId Trade::source_order() const
    {
        return f_source_order;
    }

    OrderId Trade::matched_order() const
    {
        return f_matched_order;
    }
} // namespace core::protocol::trading
