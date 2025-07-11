#ifndef TYPES_H
#define TYPES_H

namespace orderbook
{
        using OrderId = uint64_t;
        using TradeId = uint64_t;
        using Timestamp = uint64_t;
        using Price = uint64_t;
        using Quantity = uint64_t;

        enum class Side { BUY, SELL, };
}

#endif //TYPES_H
