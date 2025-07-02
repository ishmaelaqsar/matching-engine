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
        src/server/tcp_server.h
        src/server/client.h
        src/server/tcp_connection.h)
