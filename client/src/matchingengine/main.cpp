#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <iostream>
#include <matchingengine/parser.h>
#include <matchingengine/protocol/trading/add_order.h>
#include <matchingengine/protocol/trading/cancel_order.h>
#include <matchingengine/protocol/trading/modify_order.h>
#include <matchingengine/protocol/trading/trade.h>
#include <matchingengine/protocol/view/get_book.h>
#include <matchingengine/tcp/client.h>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <variant>

static constexpr std::string_view DEFAULT_HOST = "127.0.0.1";
static constexpr std::string_view DEFAULT_PORT = "8080";

std::unordered_map<std::string, std::string>
parse_args(const int argc, char** argv)
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
    if (!args.count("host"))
        args["host"] = std::string(DEFAULT_HOST);
    if (!args.count("port"))
        args["port"] = std::string(DEFAULT_PORT);
    return args;
}

template <typename SubMessage>
static void response_handler(const SubMessage& message)
{
    BOOST_LOG_TRIVIAL(info) << "Received response: " << message;
}

int main(const int argc, char** argv)
{
    const auto args = parse_args(argc, argv);

    const std::string host = args.at("host");
    const auto port_ul = std::strtoul(args.at("port").c_str(), nullptr, 10);

    if (port_ul > 65535) {
        BOOST_LOG_TRIVIAL(error) << "Port out of range: " << port_ul;
        return 1;
    }

    const auto port = static_cast<unsigned short>(port_ul);

    boost::asio::io_context io_context;

    // Keep io_context alive even if no ops pending (until we shut it down).
    auto work_guard = boost::asio::make_work_guard(io_context);

    auto client = std::make_shared<tcp::Client>(io_context, host, port);
    client->register_handler<
        core::protocol::trading::
            AddOrderResponse>(response_handler<
                              core::protocol::trading::AddOrderResponse>);
    client->register_handler<
        core::protocol::trading::
            ModifyOrderResponse>(response_handler<
                                 core::protocol::trading::ModifyOrderResponse>);
    client->register_handler<
        core::protocol::trading::
            CancelOrderResponse>(response_handler<
                                 core::protocol::trading::CancelOrderResponse>);
    client->register_handler<
        core::protocol::trading::Trade>(response_handler<
                                        core::protocol::trading::Trade>);
    client->register_handler<
        core::protocol::view::
            GetBookResponse>(response_handler<
                             core::protocol::view::GetBookResponse>);

    // Start IO thread BEFORE issuing async operations / reading stdin.
    std::thread io_thread([&] {
        try {
            io_context.run();
        } catch (const std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << "io_context exception: " << e.what();
        } catch (...) {
            BOOST_LOG_TRIVIAL(error) << "io_context: unknown exception";
        }
    });

    try {
        client->connect(); // async connect; will complete on io_thread

        std::string line;
        while (true) {
            if (!std::getline(std::cin, line))
                break; // EOF (Ctrl+D)
            if (line == "q")
                break;

            try {
                auto request = parse_line(line);

                std::visit(
                    [&](auto&& req) -> void {
                        BOOST_LOG_TRIVIAL(debug) << "Parsed request: " << req;

                        client->request(req);
                        BOOST_LOG_TRIVIAL(info) << "client->request returned";
                    },
                    request
                );
            } catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << "Parse/send error: " << e.what();
            } catch (...) {
                BOOST_LOG_TRIVIAL(error) << "Parse/send unknown exception";
            }
        }

        client->disconnect(
        ); // closes socket; pending ops will complete with errors
        work_guard.reset(
        ); // allow io_context.run() to exit once all handlers finish
        io_thread.join();
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Fatal error: " << e.what();
        work_guard.reset();
        if (io_thread.joinable())
            io_thread.join();
        return 1;
    }

    return 0;
}
