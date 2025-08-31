#ifndef TRADE_H
#define TRADE_H

#include <vector>

#include "common/protocol/message.h"
#include "common/protocol/serialize_helper.h"
#include "common/types.h"

namespace common::protocol::trading
{
        class Trade final : Message
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

                void serialize(unsigned char *data) const override
                {
                        size_t offset = 0;
                        serialize_uint64(f_id, data, &offset);
                        serialize_uint64(f_price, data, &offset);
                        serialize_uint64(f_quantity, data, &offset);
                        serialize_uint64(f_timestamp, data, &offset);
                        serialize_uint64(f_source_order, data, &offset);
                        serialize_uint64(f_matched_order, data, &offset);
                }

                void deserialize(const unsigned char *data) override
                {
                        size_t offset = 0;
                        f_id = deserialize_uint64(data, &offset);
                        f_price = deserialize_uint64(data, &offset);
                        f_quantity = deserialize_uint64(data, &offset);
                        f_timestamp = deserialize_uint64(data, &offset);
                        f_source_order = deserialize_uint64(data, &offset);
                        f_matched_order = deserialize_uint64(data, &offset);
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

                friend std::ostream &operator<<(std::ostream &os, const Trade &request)
                {
                        os << "Trade{";
                        os << "id: " << request.f_id << ", ";
                        os << "price: " << request.f_price << ", ";
                        os << "quantity: " << request.f_quantity << ", ";
                        os << "timestamp: " << request.f_timestamp << ", ";
                        os << "source_order: " << request.f_source_order << ", ";
                        os << "matched_order: " << request.f_matched_order;
                        os << "}";
                        return os;
                }

        private:
                TradeId f_id{};
                Price f_price{};
                Quantity f_quantity{};
                Timestamp f_timestamp{};
                OrderId f_source_order{};
                OrderId f_matched_order{};
        };

        class Trades final : Message
        {
        public:
                Trades() = default;
                Trades(const Trades &trades) = default;
                Trades(Trades &&trades) = default;
                Trades &operator=(const Trades &trades) = default;
                Trades &operator=(Trades &&trades) = default;
                ~Trades() override = default;

                Trades(const std::initializer_list<Trade> init) : f_trades(init)
                {}

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                [[nodiscard]] size_t size() const override;

                [[nodiscard]] MessageType type() const override
                {
                        return MessageType::Trade;
                }

                [[nodiscard]] std::vector<Trade> trades() const
                {
                        return f_trades;
                }

        private:
                std::vector<Trade> f_trades{};
        };
} // namespace common::protocol::trading

#endif // TRADE_H
