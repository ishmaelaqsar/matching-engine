#pragma once

#include <algorithm>
#include <matchingengine/protocol/trading/add_order.h>
#include <matchingengine/protocol/trading/cancel_order.h>
#include <matchingengine/protocol/trading/modify_order.h>
#include <matchingengine/protocol/view/get_book.h>
#include <sstream>
#include <variant>

using AnyRequest = std::variant<
    core::protocol::view::GetBookRequest,
    core::protocol::trading::AddOrderRequest,
    core::protocol::trading::ModifyOrderRequest,
    core::protocol::trading::CancelOrderRequest>;

static core::protocol::view::GetBookRequest
get_book_request(const std::string_view line)
{
    std::istringstream iss{std::string(line)};
    std::string symbol;
    if (!(iss >> symbol))
        throw std::invalid_argument("missing symbol in: " + std::string(line));
    return core::protocol::view::GetBookRequest(std::move(symbol));
}

static core::protocol::trading::AddOrderRequest
add_order_request(const std::string_view line)
{
    std::istringstream iss{std::string(line)};
    std::string symbol;
    std::uint64_t price, quantity, raw_side;
    if (!(iss >> symbol >> price >> quantity >> raw_side)) {
        throw std::invalid_argument(
            "invalid add order input: " + std::string(line)
        );
    }
    core::Side side;
    switch (raw_side) {
    case 1:
        side = core::Side::Buy;
        break;
    case 2:
        side = core::Side::Sell;
        break;
    default:
        throw std::invalid_argument(
            "invalid side: " + std::to_string(raw_side)
        );
    }
    return {std::move(symbol), price, quantity, side};
}

static core::protocol::trading::ModifyOrderRequest
modify_order_request(const std::string_view line)
{
    std::istringstream iss{std::string(line)};
    std::string symbol;
    std::uint64_t order_id, price, quantity;
    if (!(iss >> symbol >> order_id >> price >> quantity)) {
        throw std::invalid_argument(
            "invalid modify order input: " + std::string(line)
        );
    }
    return {std::move(symbol), order_id, quantity};
}

static core::protocol::trading::CancelOrderRequest
cancel_order_request(const std::string_view line)
{
    std::istringstream iss{std::string(line)};
    std::string symbol;
    std::uint64_t order_id;
    if (!(iss >> symbol >> order_id)) {
        throw std::invalid_argument(
            "invalid cancel order input: " + std::string(line)
        );
    }
    return {std::move(symbol), order_id};
}

[[maybe_unused]] static AnyRequest parse_line(const std::string_view line)
{
    std::istringstream iss{std::string(line)};

    std::string type;
    if (!(iss >> type))
        throw std::invalid_argument("empty request line");

    std::transform(
        type.begin(), type.end(), type.begin(),
        [](const unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    std::string rest;
    std::getline(iss, rest);

    if (!rest.empty() && rest.front() == ' ')
        rest.erase(0, 1);

    if (type == "get")
        return get_book_request(rest);
    if (type == "add")
        return add_order_request(rest);
    if (type == "modify")
        return modify_order_request(rest);
    if (type == "cancel")
        return cancel_order_request(rest);

    throw std::invalid_argument("unknown request type: " + type);
}
