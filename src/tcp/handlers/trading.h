#ifndef TRADING_MESSAGE_HANDLERS_H
#define TRADING_MESSAGE_HANDLERS_H

#include <boost/log/trivial.hpp>
#include <boost/asio/write.hpp>
#include <chrono>
#include <iostream>

#include "../../common/protocol/trading/add_order.h"

namespace tcp::handlers
{
        class TradingMessageHandler
        {
        public:
                static common::protocol::trading::AddOrderResponse
                handle_add_order_request(const common::protocol::trading::AddOrderRequest &request)
                {
                        BOOST_LOG_TRIVIAL(info) << "Received << " << request;

                        common::protocol::trading::AddOrderResponse response{};

                        response = {12345,
                                    static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count())};

                        return response;
                }
        };
} // namespace tcp::handlers

#endif // TRADING_MESSAGE_HANDLERS_H
