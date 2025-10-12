#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/protocol/trading/modify_order.h>
#include <sstream>

namespace core::protocol::trading
{
    ModifyOrderRequest::ModifyOrderRequest(
        Symbol symbol, const OrderId order_id, const Quantity quantity
    )
        : f_symbol(std::move(symbol)), f_order_id(order_id),
          f_quantity(quantity)
    {
    }

    MessageType ModifyOrderRequest::type()
    {
        return MessageType::ModifyOrderRequest;
    }

    void ModifyOrderRequest::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_symbol, dst, &offset);
        serialize_uint64(f_order_id, dst, &offset);
        serialize_uint64(f_quantity, dst, &offset);
    }

    void ModifyOrderRequest::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_symbol = deserialize_string(src, &offset);
        f_order_id = deserialize_uint64(src, &offset);
        f_quantity = deserialize_uint64(src, &offset);
    }

    void ModifyOrderRequest::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string ModifyOrderRequest::to_string() const
    {
        std::ostringstream os;
        os << "ModifyOrderRequest{";
        os << "symbol: " << f_symbol << ", ";
        os << "order_id: " << f_order_id << ", ";
        os << "quantity: " << f_quantity << ", ";
        os << "}";
        return os.str();
    }

    size_t ModifyOrderRequest::size() const
    {
        return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) +
               sizeof(f_quantity);
    }

    Symbol ModifyOrderRequest::symbol() const
    {
        return f_symbol;
    }

    OrderId ModifyOrderRequest::order_id() const
    {
        return f_order_id;
    }

    Quantity ModifyOrderRequest::quantity() const
    {
        return f_quantity;
    }

    ModifyOrderResponse::ModifyOrderResponse(
        Symbol symbol, const OrderId order_id, const Quantity quantity,
        const bool success
    )
        : f_symbol(std::move(symbol)), f_order_id(order_id),
          f_quantity(quantity), f_success(success)
    {
    }

    MessageType ModifyOrderResponse::type()
    {
        return MessageType::ModifyOrderResponse;
    }

    void ModifyOrderResponse::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_symbol, dst, &offset);
        serialize_uint64(f_order_id, dst, &offset);
        serialize_uint64(f_quantity, dst, &offset);
        serialize_uint8(f_success, dst, &offset);
    }

    void ModifyOrderResponse::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_symbol = deserialize_string(src, &offset);
        f_order_id = deserialize_uint64(src, &offset);
        f_quantity = deserialize_uint64(src, &offset);
        f_success = static_cast<bool>(deserialize_uint8(src, &offset));
    }

    void ModifyOrderResponse::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string ModifyOrderResponse::to_string() const
    {
        std::ostringstream os;
        os << "ModifyOrderResponse{";
        os << "symbol: " << f_symbol << ", ";
        os << "order_id: " << f_order_id << ", ";
        os << "quantity: " << f_quantity << ", ";
        os << "success: " << f_success;
        os << "}";
        return os.str();
    }

    size_t ModifyOrderResponse::size() const
    {
        return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) +
               sizeof(f_quantity) + sizeof(f_success);
    }

    Symbol ModifyOrderResponse::symbol() const
    {
        return f_symbol;
    }

    OrderId ModifyOrderResponse::order_id() const
    {
        return f_order_id;
    }

    Quantity ModifyOrderResponse::quantity() const
    {
        return f_quantity;
    }

    bool ModifyOrderResponse::success() const
    {
        return f_success;
    }
} // namespace core::protocol::trading
