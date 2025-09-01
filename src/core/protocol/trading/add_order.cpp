#include "core/protocol/trading/add_order.h"
#include "core/protocol/serialize_helper.h"

namespace core::protocol::trading
{
        inline AddOrderRequest::AddOrderRequest(Symbol symbol, const Price price, const Quantity quantity,
                                                const Side side) :
            f_symbol(std::move(symbol)), f_price(price), f_quantity(quantity), f_side(side)
        {}

        inline void AddOrderRequest::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_string(f_symbol, dst, &offset);
                serialize_uint64(f_price, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
                serialize_uint8(static_cast<uint8_t>(f_side), dst, &offset);
        }

        inline void AddOrderRequest::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                f_symbol = deserialize_string(src, &offset);
                f_price = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
                f_side = static_cast<Side>(deserialize_uint8(src, &offset));
        }

        inline void AddOrderRequest::print(std::ostream &os) const
        {
                os << "AddOrderRequest{";
                os << "symbol: " << f_symbol << ", ";
                os << "price: " << f_price << ", ";
                os << "quantity: " << f_quantity << ", ";
                os << "side: " << f_side;
                os << "}";
        }

        AddOrderResponse::AddOrderResponse(const OrderId order_id, const Timestamp timestamp) :
            f_order_id(order_id), f_timestamp(timestamp)
        {}

        void AddOrderResponse::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint64(f_timestamp, dst, &offset);
        }

        void AddOrderResponse::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                f_order_id = deserialize_uint64(src, &offset);
                f_timestamp = deserialize_uint64(src, &offset);
        }

        void AddOrderResponse::print(std::ostream &os) const
        {
                os << "AddOrderResponse{";
                os << "order_id: " << f_order_id << ", ";
                os << "timestamp: " << f_timestamp;
                os << "}";
        }
} // namespace core::protocol::trading
