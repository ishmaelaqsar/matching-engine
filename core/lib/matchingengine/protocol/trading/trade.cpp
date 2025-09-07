#include <matchingengine/protocol/trading/trade.h>

#include <sstream>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol::trading
{

        Trade::Trade(Symbol symbol, const TradeId id, const Price price, const Quantity quantity,
                     const Timestamp timestamp, const OrderId source_order, const OrderId matched_order) :
            f_symbol(std::move(symbol)),
            f_id(id),
            f_price(price),
            f_quantity(quantity),
            f_timestamp(timestamp),
            f_source_order(source_order),
            f_matched_order(matched_order)
        {}
        auto Trade::type() -> MessageType
        {
                return MessageType::Trade;
        }

        auto Trade::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_uint64(f_id, dst, &offset);
                serialize_uint64(f_price, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
                serialize_uint64(f_timestamp, dst, &offset);
                serialize_uint64(f_source_order, dst, &offset);
                serialize_uint64(f_matched_order, dst, &offset);
        }

        auto Trade::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_id = deserialize_uint64(src, &offset);
                f_price = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
                f_timestamp = deserialize_uint64(src, &offset);
                f_source_order = deserialize_uint64(src, &offset);
                f_matched_order = deserialize_uint64(src, &offset);
        }

        auto Trade::to_string() const -> std::string
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

        auto Trade::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto Trade::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.length() + sizeof(f_id) + sizeof(f_price) + sizeof(f_quantity) +
                       sizeof(f_timestamp) + sizeof(f_source_order) + sizeof(f_matched_order);
        }

        auto Trade::symbol() const -> const Symbol &
        {
                return f_symbol;
        }

        auto Trade::id() const -> TradeId
        {
                return f_id;
        }

        auto Trade::price() const -> Price
        {
                return f_price;
        }

        auto Trade::quantity() const -> Quantity
        {
                return f_quantity;
        }

        auto Trade::timestamp() const -> Timestamp
        {
                return f_timestamp;
        }

        auto Trade::source_order() const -> OrderId
        {
                return f_source_order;
        }

        auto Trade::matched_order() const -> OrderId
        {
                return f_matched_order;
        }
} // namespace core::protocol::trading
