#include <matchingengine/protocol/serialize_helper.h>
#include <matchingengine/protocol/trading/cancel_order.h>
#include <sstream>

namespace core::protocol::trading
{
    CancelOrderRequest::CancelOrderRequest(
        Symbol symbol, const OrderId order_id
    )
        : f_symbol(std::move(symbol)), f_order_id(order_id)
    {
    }

    MessageType CancelOrderRequest::type()
    {
        return MessageType::CancelOrderRequest;
    }

    void CancelOrderRequest::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_symbol, dst, &offset);
        serialize_uint64(f_order_id, dst, &offset);
    }

    void CancelOrderRequest::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_symbol = deserialize_string(src, &offset);
        f_order_id = deserialize_uint64(src, &offset);
    }

    void CancelOrderRequest::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string CancelOrderRequest::to_string() const
    {
        std::ostringstream os;
        os << "CancelOrderRequest{";
        os << "symbol: " << f_symbol << ", ";
        os << "order_id: " << f_order_id;
        os << "}";
        return os.str();
    }

    size_t CancelOrderRequest::size() const
    {
        return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id);
    }

    Symbol CancelOrderRequest::symbol() const
    {
        return f_symbol;
    }

    OrderId CancelOrderRequest::order_id() const
    {
        return f_order_id;
    }

    CancelOrderResponse::CancelOrderResponse(
        Symbol symbol, const OrderId order_id, const bool success
    )
        : f_symbol(std::move(symbol)), f_order_id(order_id), f_success(success)
    {
    }

    MessageType CancelOrderResponse::type()
    {
        return MessageType::CancelOrderResponse;
    }

    void CancelOrderResponse::serialize(unsigned char* dst) const
    {
        size_t offset = 0;
        serialize_string(f_symbol, dst, &offset);
        serialize_uint64(f_order_id, dst, &offset);
        serialize_uint8(f_success, dst, &offset);
    }

    void CancelOrderResponse::deserialize(const unsigned char* src)
    {
        size_t offset = 0;
        f_symbol = deserialize_string(src, &offset);
        f_order_id = deserialize_uint64(src, &offset);
        f_success = static_cast<bool>(deserialize_uint8(src, &offset));
    }

    void CancelOrderResponse::print(std::ostream& os) const
    {
        os << to_string();
    }

    std::string CancelOrderResponse::to_string() const
    {
        std::ostringstream os;
        os << "CancelOrderResponse{";
        os << "symbol: " << f_symbol << ", ";
        os << "order_id: " << f_order_id << ", ";
        os << "success: " << f_success;
        os << "}";
        return os.str();
    }

    size_t CancelOrderResponse::size() const
    {
        return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) +
               sizeof(f_success);
    }

    Symbol CancelOrderResponse::symbol() const
    {
        return f_symbol;
    }

    OrderId CancelOrderResponse::order_id() const
    {
        return f_order_id;
    }

    bool CancelOrderResponse::success() const
    {
        return f_success;
    }
} // namespace core::protocol::trading
