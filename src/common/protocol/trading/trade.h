#ifndef PROTOCOL_TRADE_H
#define PROTOCOL_TRADE_H

#include "common/protocol/message.h"
#include "common/protocol/serialize_helper.h"
#include "common/types.h"

namespace common::protocol::trading
{
        class Trade final : public Message
        {
        public:
                Trade() = default;
                Trade(const Trade &trade) = default;
                Trade(Trade &&trade) = default;
                Trade &operator=(const Trade &trade) = default;
                Trade &operator=(Trade &&trade) = default;
                ~Trade() override = default;

                Trade(const TradeId id, const Price price, const Quantity quantity, const Timestamp timestamp,
                      const OrderId source_order, const OrderId matched_order) :
                    f_id(id),
                    f_price(price),
                    f_quantity(quantity),
                    f_timestamp(timestamp),
                    f_source_order(source_order),
                    f_matched_order(matched_order)
                {}

                void serialize(unsigned char *dst) const override
                {
                        size_t offset = 0;
                        serialize_uint64(f_id, dst, &offset);
                        serialize_uint64(f_price, dst, &offset);
                        serialize_uint64(f_quantity, dst, &offset);
                        serialize_uint64(f_timestamp, dst, &offset);
                        serialize_uint64(f_source_order, dst, &offset);
                        serialize_uint64(f_matched_order, dst, &offset);
                }

                void deserialize(const unsigned char *src) override
                {
                        size_t offset = 0;
                        f_id = deserialize_uint64(src, &offset);
                        f_price = deserialize_uint64(src, &offset);
                        f_quantity = deserialize_uint64(src, &offset);
                        f_timestamp = deserialize_uint64(src, &offset);
                        f_source_order = deserialize_uint64(src, &offset);
                        f_matched_order = deserialize_uint64(src, &offset);
                }

                void print(std::ostream &os) const override
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

                [[nodiscard]] size_t size() const override
                {
                        return sizeof(f_id) + sizeof(f_price) + sizeof(f_quantity) + sizeof(f_timestamp) +
                               sizeof(f_source_order) + sizeof(f_matched_order);
                }

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::Trade;
                }

                [[nodiscard]] TradeId id() const
                {
                        return f_id;
                }

                [[nodiscard]] Price price() const
                {
                        return f_price;
                }

                [[nodiscard]] Quantity quantity() const
                {
                        return f_quantity;
                }

                [[nodiscard]] Timestamp timestamp() const
                {
                        return f_timestamp;
                }

                [[nodiscard]] OrderId source_order() const
                {
                        return f_source_order;
                }

                [[nodiscard]] OrderId matched_order() const
                {
                        return f_matched_order;
                }

        private:
                TradeId f_id{};
                Price f_price{};
                Quantity f_quantity{};
                Timestamp f_timestamp{};
                OrderId f_source_order{};
                OrderId f_matched_order{};
        };
} // namespace common::protocol::trading

#endif // PROTOCOL_TRADE_H
