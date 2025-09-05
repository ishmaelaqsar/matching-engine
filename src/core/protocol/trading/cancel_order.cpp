#include <core/protocol/serialize_helper.h>
#include <core/protocol/trading/cancel_order.h>

namespace core::protocol::trading
{
        CancelOrderRequest::CancelOrderRequest(Symbol symbol, const OrderId order_id) :
            f_symbol(std::move(symbol)), f_order_id(order_id)
        {}

        void CancelOrderRequest::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_order_id, dst, &offset);
        }

        void CancelOrderRequest::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_order_id = deserialize_uint64(src, &offset);
        }

        void CancelOrderRequest::print(std::ostream &os) const
        {
                os << "CancelOrderRequest{";
                os << "symbol: " << f_symbol << ", ";
                os << "order_id: " << f_order_id;
                os << "}";
        }

        CancelOrderResponse::CancelOrderResponse(Symbol symbol, const OrderId order_id, const bool success) :
            f_symbol(std::move(symbol)), f_order_id(order_id), f_success(success)
        {}

        void CancelOrderResponse::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint8(f_success, dst, &offset);
        }

        void CancelOrderResponse::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_order_id = deserialize_uint64(src, &offset);
                f_success = static_cast<bool>(deserialize_uint8(src, &offset));
        }

        void CancelOrderResponse::print(std::ostream &os) const
        {
                os << "CancelOrderResponse{";
                os << "symbol: " << f_symbol << ", ";
                os << "order_id: " << f_order_id << ", ";
                os << "success: " << f_success;
                os << "}";
        }
} // namespace core::protocol::trading
