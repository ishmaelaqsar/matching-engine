#pragma once

#include "core/protocol/message.h"
#include "core/types.h"

namespace core::protocol::trading
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

                Trade(TradeId id, Price price, Quantity quantity, Timestamp timestamp, OrderId source_order,
                      OrderId matched_order);

                void serialize(unsigned char *dst) const override;

                void deserialize(const unsigned char *src) override;

                void print(std::ostream &os) const override;

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
} // namespace core::protocol::trading
