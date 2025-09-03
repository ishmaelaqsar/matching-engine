#include <algorithm>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "core/protocol/trading/add_order.h"
#include "core/protocol/view/get_book.h"
#include "tcp/client.h"

static constexpr std::string_view DEFAULT_HOST = "127.0.0.1";
static constexpr std::string_view DEFAULT_PORT = "8080";

std::unordered_map<std::string, std::string> parse_args(const int argc, char **argv)
{
        std::unordered_map<std::string, std::string> args{};
        for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (const auto pos = arg.find(':'); pos != std::string::npos) {
                        const auto key = arg.substr(0, pos);
                        const auto value = arg.substr(pos + 1);
                        args[key] = value;
                }
        }
        if (args.find("host") == args.end()) {
                args["host"] = DEFAULT_HOST;
        }
        if (args.find("port") == args.end()) {
                args["port"] = DEFAULT_PORT;
        }
        return args;
}

core::protocol::view::GetBookRequest get_book_request(const std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string symbol;
        if (!(iss >> symbol)) {
                throw std::invalid_argument("missing symbol in: " + std::string(line));
        }
        return core::protocol::view::GetBookRequest(std::move(symbol));
}

core::protocol::trading::AddOrderRequest add_order_request(const std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string symbol;
        uint64_t price, quantity, raw_side;

        if (!(iss >> symbol >> price >> quantity >> raw_side)) {
                throw std::invalid_argument("invalid add order input: " + std::string(line));
        }

        core::Side side;
        switch (raw_side) {
                case 1: side = core::Side::Buy; break;
                case 2: side = core::Side::Sell; break;
                default: throw std::invalid_argument("invalid side: " + std::to_string(raw_side));
        }

        return {std::move(symbol), price, quantity, side};
}

std::unique_ptr<core::protocol::Message> parse_line(const std::string_view line)
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
                return std::make_unique<core::protocol::view::GetBookRequest>(get_book_request(rest));
        }
        if (type == "add") {
                return std::make_unique<core::protocol::trading::AddOrderRequest>(add_order_request(rest));
        }

        throw std::invalid_argument("unknown request type: " + type);
}

int main(const int argc, char **argv)
{
        boost::asio::io_context io;
        const auto args = parse_args(argc, argv);
        const std::string host = args.at("host");
        const unsigned short port = strtoul(args.at("port").c_str(), nullptr, 10);
        tcp::Client client(io, host, port, [](const core::protocol::Message &response) {
                BOOST_LOG_TRIVIAL(info) << "Received response: " << response;
        });

        try {
                client.connect();
                std::string line;
                while (true) {
                        std::getline(std::cin, line);
                        if (line == "q") break;

                        std::unique_ptr<core::protocol::Message> request = parse_line(line);

                        client.request(*request);
                }
                client.disconnect();
        } catch (const std::exception &e) {
                BOOST_LOG_TRIVIAL(error) << "Error: " << e.what();
        }
}
