#include <iostream>

#include "src/common/protocol/trading/add_order.h"
#include "src/tcp/client.h"

common::AddOrderRequest parse_line(std::string &line);

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

                        const auto request = parse_line(line);

                        auto [order_id, timestamp] = client.add_order(request);
                        std::cout << "Order ID: " << order_id << std::endl;
                        std::cout << "Timestamp: " << timestamp << std::endl;
                }
                client.disconnect();
        } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
        }
}

common::AddOrderRequest parse_line(std::string &line)
{
        const auto symbol_position = line.find(' ', 0);
        if (symbol_position == std::string::npos) {
                throw std::invalid_argument("1: invalid input: " + line);
        }

        const auto price_position = line.find(' ', symbol_position + 1);
        if (price_position == std::string::npos) {
                throw std::invalid_argument("2: invalid input: " + line);
        }

        const auto quantity_position = line.find(' ', price_position + 1);
        if (quantity_position == std::string::npos) {
                throw std::invalid_argument("3: invalid input: " + line);
        }

        const auto symbol = std::string(&line[0], &line[symbol_position]);
        const uint64_t price = std::stoull(std::string(&line[symbol_position + 1], &line[price_position]));
        const uint64_t quantity = std::stoull(std::string(&line[price_position + 1], &line[quantity_position]));
        const uint8_t raw_side = std::stoull(std::string(&line[quantity_position + 1]));

        common::Side side;
        switch (raw_side) {
                case 1: side = common::Side::Buy;
                        break;
                case 2: side = common::Side::Sell;
                        break;
                default: throw std::invalid_argument("invalid side: " + std::to_string(raw_side));
        }

        return {symbol, price, quantity, side};
}
