#include "core/protocol/trading/modify_order.h"
#include "core/protocol/serialize_helper.h"

namespace core::protocol::trading
{
        ModifyOrderRequest::ModifyOrderRequest(const OrderId order_id, const Price price, const Quantity quantity) :
            f_order_id(order_id), f_price(price), f_quantity(quantity)
        {}

        void ModifyOrderRequest::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint64(f_price, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
        }

        void ModifyOrderRequest::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                f_order_id = deserialize_uint64(src, &offset);
                f_price = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
        }

        void ModifyOrderRequest::print(std::ostream &os) const
        {
                os << "ModifyOrderRequest{";
                os << "order_id: " << f_order_id << ", ";
                os << "price: " << f_price << ", ";
                os << "quantity: " << f_quantity << ", ";
                os << "}";
        }

        ModifyOrderResponse::ModifyOrderResponse(const OrderId order_id, const Price price, const Quantity quantity,
                                                 const bool success) :
            f_order_id(order_id), f_price(price), f_quantity(quantity), f_success(success)
        {}

        void ModifyOrderResponse::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_uint64(f_order_id, dst, &offset);
                serialize_uint64(f_price, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
                serialize_uint8(f_success, dst, &offset);
        }

        void ModifyOrderResponse::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                f_order_id = deserialize_uint64(src, &offset);
                f_price = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
                f_success = static_cast<bool>(deserialize_uint8(src, &offset));
        }

        void ModifyOrderResponse::print(std::ostream &os) const
        {
                os << "ModifyOrderResponse{";
                os << "order_id: " << f_order_id << ", ";
                os << "price: " << f_price << ", ";
                os << "quantity: " << f_quantity << ", ";
                os << "success: " << f_success;
                os << "}";
        }
} // namespace core::protocol::trading
