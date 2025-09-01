#include <algorithm>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "common/protocol/trading/add_order.h"
#include "common/protocol/view/get_book.h"
#include "tcp/client.h"

common::protocol::view::GetBookRequest get_book_request(const std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string symbol;
        if (!(iss >> symbol)) {
                throw std::invalid_argument("missing symbol in: " + std::string(line));
        }
        return common::protocol::view::GetBookRequest(std::move(symbol));
}

common::protocol::trading::AddOrderRequest add_order_request(const std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string symbol;
        uint64_t price, quantity, raw_side;

        if (!(iss >> symbol >> price >> quantity >> raw_side)) {
                throw std::invalid_argument("invalid add order input: " + std::string(line));
        }

        common::Side side;
        switch (raw_side) {
                case 1: side = common::Side::Buy; break;
                case 2: side = common::Side::Sell; break;
                default: throw std::invalid_argument("invalid side: " + std::to_string(raw_side));
        }

        return {std::move(symbol), price, quantity, side};
}

std::unique_ptr<common::protocol::Message> parse_line(std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string type;
        if (!(iss >> type)) {
                throw std::invalid_argument("empty request line");
        }

        std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c) { return std::tolower(c); });

        std::string rest;
        std::getline(iss, rest);
        if (!rest.empty() && rest.front() == ' ') rest.erase(0, 1);

        if (type == "get") {
                return std::make_unique<common::protocol::view::GetBookRequest>(get_book_request(rest));
        }
        if (type == "add") {
                return std::make_unique<common::protocol::trading::AddOrderRequest>(add_order_request(rest));
        }

        throw std::invalid_argument("unknown request type: " + type);
}

int main()
{
        boost::asio::io_context io;
        tcp::Client client(io, "127.0.0.1", 8080);

        try {
                client.connect();
                std::string line;
                while (true) {
                        std::getline(std::cin, line);
                        if (line == "q") break;

                        std::unique_ptr<common::protocol::Message> request = parse_line(line);

                        std::shared_ptr<common::protocol::Message> response = client.request(request);
                        BOOST_LOG_TRIVIAL(info) << "Response: " << *response;
                }
                client.disconnect();
        } catch (const std::exception &e) {
                BOOST_LOG_TRIVIAL(error) << "Error: " << e.what();
        }
}
