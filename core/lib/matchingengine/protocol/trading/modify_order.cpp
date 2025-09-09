#include <matchingengine/protocol/trading/modify_order.h>

#include <sstream>

#include <matchingengine/protocol/serialize_helper.h>

namespace core::protocol::trading
{
        ModifyOrderRequest::ModifyOrderRequest(Symbol symbol, const OrderId order_id, const Quantity quantity) :
            f_symbol(std::move(symbol)), f_order_id(order_id), f_quantity(quantity)
        {}

        auto ModifyOrderRequest::type() -> MessageType
        {
                return MessageType::ModifyOrderRequest;
        }

        auto ModifyOrderRequest::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
        }

        auto ModifyOrderRequest::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_order_id = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
        }

        auto ModifyOrderRequest::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto ModifyOrderRequest::to_string() const -> std::string
        {
                std::ostringstream os;
                os << "ModifyOrderRequest{";
                os << "symbol: " << f_symbol << ", ";
                os << "order_id: " << f_order_id << ", ";
                os << "quantity: " << f_quantity << ", ";
                os << "}";
                return os.str();
        }

        auto ModifyOrderRequest::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) + sizeof(f_quantity);
        }

        auto ModifyOrderRequest::symbol() const -> Symbol
        {
                return f_symbol;
        }

        auto ModifyOrderRequest::order_id() const -> OrderId
        {
                return f_order_id;
        }

        auto ModifyOrderRequest::quantity() const -> Quantity
        {
                return f_quantity;
        }

        ModifyOrderResponse::ModifyOrderResponse(Symbol symbol, const OrderId order_id, const Quantity quantity, const bool success) :
            f_symbol(std::move(symbol)), f_order_id(order_id), f_quantity(quantity), f_success(success)
        {}

        auto ModifyOrderResponse::type() -> MessageType
        {
                return MessageType::ModifyOrderResponse;
        }

        auto ModifyOrderResponse::serialize(unsigned char *dst) const -> void
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
                serialize_uint8(f_success, dst, &offset);
        }

        auto ModifyOrderResponse::deserialize(const unsigned char *src) -> void
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_order_id = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
                f_success = static_cast<bool>(deserialize_uint8(src, &offset));
        }

        auto ModifyOrderResponse::print(std::ostream &os) const -> void
        {
                os << to_string();
        }

        auto ModifyOrderResponse::to_string() const -> std::string
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

        auto ModifyOrderResponse::size() const -> size_t
        {
                return sizeof(StrLen) + f_symbol.length() + sizeof(f_order_id) + sizeof(f_quantity) + sizeof(f_success);
        }

        auto ModifyOrderResponse::symbol() const -> Symbol
        {
                return f_symbol;
        }

        auto ModifyOrderResponse::order_id() const -> OrderId
        {
                return f_order_id;
        }

        auto ModifyOrderResponse::quantity() const -> Quantity
        {
                return f_quantity;
        }

        auto ModifyOrderResponse::success() const -> bool
        {
                return f_success;
        }
} // namespace core::protocol::trading
