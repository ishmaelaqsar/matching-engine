#pragma once

#include <matchingengine/protocol/message.h>
#include <matchingengine/types.h>

namespace core::protocol::trading
{
    class Trade final : public Message<Trade>
    {
    public:
        Trade() = default;
        Trade(const Trade& trade) = default;
        Trade(Trade&& trade) = default;
        Trade& operator=(const Trade& trade) = default;
        Trade& operator=(Trade&& trade) = default;
        ~Trade() = default;

        Trade(
            Symbol symbol, TradeId id, Price price, Quantity quantity,
            Timestamp timestamp, OrderId source_order, OrderId matched_order
        );

        static MessageType type();

        void serialize(unsigned char* dst) const;

        void deserialize(const unsigned char* src);

        void print(std::ostream& os) const;

        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] const Symbol& symbol() const;

        [[nodiscard]] TradeId id() const;

        [[nodiscard]] Price price() const;

        [[nodiscard]] Quantity quantity() const;

        [[nodiscard]] Timestamp timestamp() const;

        [[nodiscard]] OrderId source_order() const;

        [[nodiscard]] OrderId matched_order() const;

    private:
        Symbol f_symbol;
        TradeId f_id{};
        Price f_price{};
        Quantity f_quantity{};
        Timestamp f_timestamp{};
        OrderId f_source_order{};
        OrderId f_matched_order{};
    };
} // namespace core::protocol::trading
