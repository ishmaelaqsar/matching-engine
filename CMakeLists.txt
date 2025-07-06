cmake_minimum_required(VERSION 3.25)
project(matching_engine)

set(CMAKE_CXX_STANDARD 17)

add_executable(matching_engine main.cpp
        src/book/types.h
        src/book/book.h
        src/book/order.h
        src/book/trade.h
        src/book/level.h
        src/book/shared_counter.h
        src/tcp/server.h
        src/tcp/connection.h
        src/tcp/protocol/header.h
        src/tcp/protocol/add_order.h
)

add_executable(tcp_server server.cpp
        src/tcp/server.h
        src/tcp/connection.h
        src/tcp/protocol/header.h
        src/tcp/protocol/add_order.h
)

add_executable(tcp_client client.cpp
        src/tcp/client.h
        src/tcp/protocol/header.h
        src/tcp/protocol/add_order.h
)