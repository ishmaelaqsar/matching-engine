#include <matchingengine/protocol/trading/add_order.h>

#include <sstream>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol::trading
{
        AddOrderRequest::AddOrderRequest(Symbol symbol, const Price price, const Quantity quantity, const Side side) :
            f_symbol(std::move(symbol)), f_price(price), f_quantity(quantity), f_side(side)
        {}

        auto AddOrderRequest::type() -> MessageType
        {
                return MessageType::AddOrderRequest;
        }

        auto AddOrderRequest::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_price, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
                serialize_uint8(static_cast<uint8_t>(f_side), dst, &offset);
        }

        auto AddOrderRequest::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_price = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
                f_side = static_cast<Side>(deserialize_uint8(src, &offset));
        }

        auto AddOrderRequest::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "AddOrderRequest{";
                os << "symbol: " << f_symbol << ", ";
                os << "price: " << f_price << ", ";
                os << "quantity: " << f_quantity << ", ";
                os << "side: " << f_side;
                os << "}";
                return os.str();
        }

        auto AddOrderRequest::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto AddOrderRequest::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.length() + sizeof(f_price) + sizeof(f_quantity) + sizeof(f_side);
        }

        auto AddOrderRequest::symbol() const -> Symbol
        {
                return f_symbol;
        }

        auto AddOrderRequest::price() const -> Price
        {
                return f_price;
        }

        auto AddOrderRequest::quantity() const -> Quantity
        {
                return f_quantity;
        }

        auto AddOrderRequest::side() const -> Side
        {
                return f_side;
        }

        AddOrderResponse::AddOrderResponse(Symbol symbol, const OrderId order_id, const Price price,
                                           const Quantity quantity, const Side side, const Timestamp timestamp) :
            f_symbol(std::move(symbol)),
            f_order_id(order_id),
            f_price(price),
            f_quantity(quantity),
            f_side(side),
            f_timestamp(timestamp)
        {}

        auto AddOrderResponse::type() -> MessageType
        {
                return MessageType::AddOrderResponse;
        }

        auto AddOrderResponse::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint64(f_price, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
                serialize_uint8(static_cast<uint8_t>(f_side), dst, &offset);
                serialize_uint64(f_timestamp, dst, &offset);
        }

        auto AddOrderResponse::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_order_id = deserialize_uint64(src, &offset);
                f_price = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
                f_side = static_cast<Side>(deserialize_uint8(src, &offset));
                f_timestamp = deserialize_uint64(src, &offset);
        }

        auto AddOrderResponse::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "AddOrderResponse{";
                os << "symbol: " << f_symbol << ", ";
                os << "order_id: " << f_order_id << ", ";
                os << "price: " << f_price << ", ";
                os << "quantity: " << f_quantity << ", ";
                os << "side: " << f_side << ", ";
                os << "timestamp: " << f_timestamp;
                os << "}";
                return os.str();
        }

        auto AddOrderResponse::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto AddOrderResponse::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) + sizeof(f_price) + sizeof(f_quantity) +
                       sizeof(f_side) + sizeof(f_timestamp);
        }

        auto AddOrderResponse::symbol() const -> Symbol
        {
                return f_symbol;
        }

        auto AddOrderResponse::order_id() const -> OrderId
        {
                return f_order_id;
        }

        auto AddOrderResponse::price() const -> Price
        {
                return f_price;
        }

        auto AddOrderResponse::quantity() const -> Quantity
        {
                return f_quantity;
        }

        auto AddOrderResponse::side() const -> Side
        {
                return f_side;
        }

        auto AddOrderResponse::timestamp() const -> Timestamp
        {
                return f_timestamp;
        }
} // namespace core::protocol::trading
