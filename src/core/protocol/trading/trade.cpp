#include <core/protocol/serialize_helper.h>
#include <core/protocol/trading/trade.h>

namespace core::protocol::trading
{

        Trade::Trade(const TradeId id, const Price price, const Quantity quantity, const Timestamp timestamp,
                     const OrderId source_order, const OrderId matched_order) :
            f_id(id),
            f_price(price),
            f_quantity(quantity),
            f_timestamp(timestamp),
            f_source_order(source_order),
            f_matched_order(matched_order)
        {}

        void Trade::serialize(unsigned char *dst) const
        {
                size_t offset = 0;
                serialize_uint64(f_id, dst, &offset);
                serialize_uint64(f_price, dst, &offset);
                serialize_uint64(f_quantity, dst, &offset);
                serialize_uint64(f_timestamp, dst, &offset);
                serialize_uint64(f_source_order, dst, &offset);
                serialize_uint64(f_matched_order, dst, &offset);
        }

        void Trade::deserialize(const unsigned char *src)
        {
                size_t offset = 0;
                f_id = deserialize_uint64(src, &offset);
                f_price = deserialize_uint64(src, &offset);
                f_quantity = deserialize_uint64(src, &offset);
                f_timestamp = deserialize_uint64(src, &offset);
                f_source_order = deserialize_uint64(src, &offset);
                f_matched_order = deserialize_uint64(src, &offset);
        }

        void Trade::print(std::ostream &os) const
        {
                os << "Trade{";
                os << "id: " << f_id << ", ";
                os << "price: " << f_price << ", ";
                os << "quantity: " << f_quantity << ", ";
                os << "timestamp: " << f_timestamp << ", ";
                os << "source_order: " << f_source_order << ", ";
                os << "matched_order: " << f_matched_order;
                os << "}";
        }
} // namespace core::protocol::trading
