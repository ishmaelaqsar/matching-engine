#include <boost/asio/io_context.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include <matchingengine/engine.h>
#include <matchingengine/tcp/server.h>

void init_logging()
{
#ifdef NDEBUG
        // Release mode: only info and above
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#else
        // Debug mode: all logs
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
#endif
}

int main()
{
        init_logging();
        const auto inbound = std::make_shared<core::RingBuffer<core::Payload>>();
        const auto outbound = std::make_shared<core::RingBuffer<core::Payload>>();
        try {
                boost::asio::io_context io_context;
                const auto server = std::make_shared<tcp::Server>(inbound, outbound, io_context);
                server->start();

                std::thread engine_thread([&]() -> void {
                        auto engine = orderbook::Engine(inbound, outbound);
                        while (server->is_running()) {
                                try {
                                        if (!engine.do_work()) {
                                                std::this_thread::yield();
                                        }
                                } catch (const std::exception &e) {
                                        BOOST_LOG_TRIVIAL(error) << "Engine Thread: " << e.what();
                                } catch (...) {
                                        BOOST_LOG_TRIVIAL(error) << "Engine Thread: unknown exception";
                                }
                        }
                });

                try {
                        io_context.run();
                } catch (const std::exception &e) {
                        BOOST_LOG_TRIVIAL(error) << "io_context exception: " << e.what();
                } catch (...) {
                        BOOST_LOG_TRIVIAL(error) << "io_context: unknown exception";
                }

                engine_thread.join();
        } catch (const std::exception &e) {
                BOOST_LOG_TRIVIAL(error) << "Main exception: " << e.what();
                return 1;
        } catch (...) {
                BOOST_LOG_TRIVIAL(error) << "Main exception: unknown exception";
                return 1;
        }

        return 0;
}
