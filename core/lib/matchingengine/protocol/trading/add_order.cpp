#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/protocol/trading/add_order.h>
#include <sstream>

namespace core::protocol::trading
{
    AddOrderRequest::AddOrderRequest(
        Symbol symbol, const Price price, const Quantity quantity,
        const Side side
    )
        : f_symbol(std::move(symbol)), f_price(price), f_quantity(quantity),
          f_side(side)
    {
    }

    MessageType AddOrderRequest::type()
    {
        return MessageType::AddOrderRequest;
    }

    void AddOrderRequest::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_symbol, dst, &offset);
        serialize_uint64(f_price, dst, &offset);
        serialize_uint64(f_quantity, dst, &offset);
        serialize_uint8(static_cast<uint8_t>(f_side), dst, &offset);
    }

    void AddOrderRequest::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_symbol = deserialize_string(src, &offset);
        f_price = deserialize_uint64(src, &offset);
        f_quantity = deserialize_uint64(src, &offset);
        f_side = static_cast<Side>(deserialize_uint8(src, &offset));
    }

    void AddOrderRequest::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string AddOrderRequest::to_string() const
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

    size_t AddOrderRequest::size() const
    {
        return sizeof(StrLen) + f_symbol.length() + sizeof(f_price) +
               sizeof(f_quantity) + sizeof(f_side);
    }

    Symbol AddOrderRequest::symbol() const
    {
        return f_symbol;
    }

    Price AddOrderRequest::price() const
    {
        return f_price;
    }

    Quantity AddOrderRequest::quantity() const
    {
        return f_quantity;
    }

    Side AddOrderRequest::side() const
    {
        return f_side;
    }

    AddOrderResponse::AddOrderResponse(
        Symbol symbol, const OrderId order_id, const Price price,
        const Quantity quantity, const Side side, const Timestamp timestamp
    )
        : f_symbol(std::move(symbol)), f_order_id(order_id), f_price(price),
          f_quantity(quantity), f_side(side), f_timestamp(timestamp)
    {
    }

    MessageType AddOrderResponse::type()
    {
        return MessageType::AddOrderResponse;
    }

    void AddOrderResponse::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_symbol, dst, &offset);
        serialize_uint64(f_order_id, dst, &offset);
        serialize_uint64(f_price, dst, &offset);
        serialize_uint64(f_quantity, dst, &offset);
        serialize_uint8(static_cast<uint8_t>(f_side), dst, &offset);
        serialize_uint64(f_timestamp, dst, &offset);
    }

    void AddOrderResponse::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_symbol = deserialize_string(src, &offset);
        f_order_id = deserialize_uint64(src, &offset);
        f_price = deserialize_uint64(src, &offset);
        f_quantity = deserialize_uint64(src, &offset);
        f_side = static_cast<Side>(deserialize_uint8(src, &offset));
        f_timestamp = deserialize_uint64(src, &offset);
    }

    void AddOrderResponse::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string AddOrderResponse::to_string() const
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

    size_t AddOrderResponse::size() const
    {
        return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) +
               sizeof(f_price) + sizeof(f_quantity) + sizeof(f_side) +
               sizeof(f_timestamp);
    }

    Symbol AddOrderResponse::symbol() const
    {
        return f_symbol;
    }

    OrderId AddOrderResponse::order_id() const
    {
        return f_order_id;
    }

    Price AddOrderResponse::price() const
    {
        return f_price;
    }

    Quantity AddOrderResponse::quantity() const
    {
        return f_quantity;
    }

    Side AddOrderResponse::side() const
    {
        return f_side;
    }

    Timestamp AddOrderResponse::timestamp() const
    {
        return f_timestamp;
    }
} // namespace core::protocol::trading
