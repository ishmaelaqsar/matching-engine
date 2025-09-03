#include <algorithm>
#include <boost/asio.hpp> // ensure io_context is declared
#include <boost/log/trivial.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

#include <core/protocol/trading/add_order.h>
#include <core/protocol/view/get_book.h>
#include <tcp/client.h>

static constexpr std::string_view DEFAULT_HOST = "127.0.0.1";
static constexpr std::string_view DEFAULT_PORT = "8080";

std::unordered_map<std::string, std::string> parse_args(int argc, char **argv)
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
        if (!args.count("host")) args["host"] = std::string(DEFAULT_HOST);
        if (!args.count("port")) args["port"] = std::string(DEFAULT_PORT);
        return args;
}

core::protocol::view::GetBookRequest get_book_request(std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string symbol;
        if (!(iss >> symbol)) throw std::invalid_argument("missing symbol in: " + std::string(line));
        return core::protocol::view::GetBookRequest(std::move(symbol));
}

core::protocol::trading::AddOrderRequest add_order_request(std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string symbol;
        std::uint64_t price, quantity, raw_side;
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

std::unique_ptr<core::protocol::Message> parse_line(std::string_view line)
{
        std::istringstream iss{std::string(line)};
        std::string type;
        if (!(iss >> type)) throw std::invalid_argument("empty request line");
        std::transform(type.begin(), type.end(), type.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::string rest;
        std::getline(iss, rest);
        if (!rest.empty() && rest.front() == ' ') rest.erase(0, 1);

        if (type == "get") return std::make_unique<core::protocol::view::GetBookRequest>(get_book_request(rest));
        if (type == "add") return std::make_unique<core::protocol::trading::AddOrderRequest>(add_order_request(rest));
        throw std::invalid_argument("unknown request type: " + type);
}

int main(int argc, char **argv)
{
        const auto args = parse_args(argc, argv);

        const std::string host = args.at("host");
        const auto port_ul = std::strtoul(args.at("port").c_str(), nullptr, 10);
        if (port_ul > 65535) {
                BOOST_LOG_TRIVIAL(error) << "Port out of range: " << port_ul;
                return 1;
        }
        const unsigned short port = static_cast<unsigned short>(port_ul);

        boost::asio::io_context io_context;

        // Keep io_context alive even if no ops pending (until we shut it down).
        auto work_guard = boost::asio::make_work_guard(io_context);

        auto client = std::make_shared<tcp::Client>(
                io_context, host, port, [](const std::shared_ptr<core::protocol::Message> &response) {
                        BOOST_LOG_TRIVIAL(info) << "Received response: " << *response;
                });

        // Start IO thread BEFORE issuing async operations / reading stdin.
        std::thread io_thread([&] {
                try {
                        io_context.run();
                } catch (const std::exception &e) {
                        BOOST_LOG_TRIVIAL(error) << "io_context exception: " << e.what();
                } catch (...) {
                        BOOST_LOG_TRIVIAL(error) << "io_context: unknown exception";
                }
        });

        try {
                client->connect(); // async connect; will complete on io_thread

                std::string line;
                while (true) {
                        if (!std::getline(std::cin, line)) break; // EOF (Ctrl+D)
                        if (line == "q") break;

                        try {
                                auto request = parse_line(line);
                                BOOST_LOG_TRIVIAL(info) << "Parsed request: " << *request;
                                client->request(*request);
                                BOOST_LOG_TRIVIAL(info) << "client->request returned";
                        } catch (const std::exception &e) {
                                BOOST_LOG_TRIVIAL(error) << "Parse/send error: " << e.what();
                        } catch (...) {
                                BOOST_LOG_TRIVIAL(error) << "Parse/send unknown exception";
                        }
                }

                client->disconnect(); // closes socket; pending ops will complete with errors
                work_guard.reset(); // allow io_context.run() to exit once all handlers finish
                io_thread.join();
        } catch (const std::exception &e) {
                BOOST_LOG_TRIVIAL(error) << "Fatal error: " << e.what();
                work_guard.reset();
                if (io_thread.joinable()) io_thread.join();
                return 1;
        }

        return 0;
}
