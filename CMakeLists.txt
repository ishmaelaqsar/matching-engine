cmake_minimum_required(VERSION 3.25)
project(matching_engine)

set(CMAKE_CXX_STANDARD 17)

set(SYMBOL_LEN 4)
add_compile_definitions(SYMBOL_LEN=${SYMBOL_LEN})

set(RINGBUFFER_CAPACITY 4096)
add_compile_definitions(RINGBUFFER_CAPACITY=${RINGBUFFER_CAPACITY})

add_executable(matching_engine main.cpp
        src/book/book.h
        src/book/order.h
        src/book/trade.h
        src/book/level.h
        src/book/shared_counter.h
        src/tcp/server.h
        src/tcp/connection.h
        src/tcp/header.h
        src/common/add_order.h
        src/common/types.h
        src/common/ring_buffer.h
        src/common/util.h
)

add_executable(tcp_server server.cpp
        src/tcp/server.h
        src/tcp/connection.h
        src/tcp/header.h
        src/common/add_order.h
        src/common/types.h
        src/common/util.h
)

add_executable(tcp_client client.cpp
        src/tcp/client.h
        src/tcp/header.h
        src/common/add_order.h
        src/common/types.h
        src/common/util.h
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
        test/tcp/header_test.cpp
        test/common/string_test.cpp
        test/common/add_order_test.cpp
        test/book/level_test.cpp
        test/book/book_test.cpp
        test/book/helpers.h
)
target_link_libraries(matching_engine_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(matching_engine_test)