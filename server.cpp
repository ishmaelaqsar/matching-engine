#include "src/tcp/server.h"

int main()
{
        try {
                boost::asio::io_context io_context;
                orderbook::TCPServer server(io_context);
                server.start_accept();
                io_context.run();
        } catch (const std::exception &e) {
                std::cerr << "Exception: " << e.what() << std::endl;
        }

        return 0;
}
