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

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(matching_engine_test
        test/tcp/protocol_test.cpp
)
target_link_libraries(matching_engine_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(matching_engine_test)