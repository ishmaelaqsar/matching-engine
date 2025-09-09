#include <matchingengine/protocol/trading/cancel_order.h>

#include <sstream>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol::trading
{
        CancelOrderRequest::CancelOrderRequest(Symbol symbol, const OrderId order_id) :
            f_symbol(std::move(symbol)), f_order_id(order_id)
        {}

        auto CancelOrderRequest::type() -> MessageType
        {
                return MessageType::CancelOrderRequest;
        }

        auto CancelOrderRequest::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_order_id, dst, &offset);
        }

        auto CancelOrderRequest::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_order_id = deserialize_uint64(src, &offset);
        }

        auto CancelOrderRequest::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto CancelOrderRequest::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "CancelOrderRequest{";
                os << "symbol: " << f_symbol << ", ";
                os << "order_id: " << f_order_id;
                os << "}";
                return os.str();
        }

        auto CancelOrderRequest::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id);
        }

        auto CancelOrderRequest::symbol() const -> Symbol
        {
                return f_symbol;
        }

        auto CancelOrderRequest::order_id() const -> OrderId
        {
                return f_order_id;
        }

        CancelOrderResponse::CancelOrderResponse(Symbol symbol, const OrderId order_id, const bool success) :
            f_symbol(std::move(symbol)), f_order_id(order_id), f_success(success)
        {}

        auto CancelOrderResponse::type() -> MessageType
        {
                return MessageType::CancelOrderResponse;
        }

        auto CancelOrderResponse::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint8(f_success, dst, &offset);
        }

        auto CancelOrderResponse::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_order_id = deserialize_uint64(src, &offset);
                f_success = static_cast<bool>(deserialize_uint8(src, &offset));
        }

        auto CancelOrderResponse::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto CancelOrderResponse::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "CancelOrderResponse{";
                os << "symbol: " << f_symbol << ", ";
                os << "order_id: " << f_order_id << ", ";
                os << "success: " << f_success;
                os << "}";
                return os.str();
        }

        auto CancelOrderResponse::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) + sizeof(f_success);
        }

        auto CancelOrderResponse::symbol() const -> Symbol
        {
                return f_symbol;
        }

        auto CancelOrderResponse::order_id() const -> OrderId
        {
                return f_order_id;
        }

        auto CancelOrderResponse::success() const -> bool
        {
                return f_success;
        }
} // namespace core::protocol::trading
