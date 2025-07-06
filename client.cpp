#include "src/tcp/client.h"

#include <iostream>

orderbook::AddOrderRequest parse_line(std::string &line);

int main()
{
        boost::asio::io_context io;
        orderbook::TCPClient client(io, "127.0.0.1", 8080);

        uint64_t price;
        uint64_t quantity;
        uint8_t side;

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

orderbook::AddOrderRequest parse_line(std::string &line)
{
        const auto price_position = line.find(' ', 0);
        if (price_position == std::string::npos) {
                throw std::invalid_argument("1: invalid input: " + line);
        }

        const auto quantity_position = line.find(' ', price_position + 1);
        if (quantity_position == std::string::npos) {
                throw std::invalid_argument("2: invalid input: " + line);
        }

        uint64_t price = std::stoull(std::string(&line[0], &line[price_position]));
        uint64_t quantity = std::stoull(std::string(&line[price_position + 1], &line[quantity_position]));
        uint8_t raw_side = std::stoull(std::string(&line[quantity_position + 1]));

        orderbook::Side side;
        switch (raw_side) {
                case 1: side = orderbook::Side::Buy;
                        break;
                case 2: side = orderbook::Side::Sell;
                        break;
                default: throw std::invalid_argument("invalid side: " + std::to_string(raw_side));
        }

        return {price, quantity, side};
}
