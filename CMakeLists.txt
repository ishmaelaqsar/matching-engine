cmake_minimum_required(VERSION 3.25)
project(matching_engine)

set(CMAKE_CXX_STANDARD 17)

# Set default book type to Currency
set(BOOK_TYPE "Currency" CACHE STRING "Type of book: Currency or Equity")
if(NOT BOOK_TYPE STREQUAL "Currency" AND
        NOT BOOK_TYPE STREQUAL "Equity")
    message(FATAL_ERROR "BOOK_TYPE must be either 'Currency' or 'Equity'")
endif()
add_compile_definitions(BOOK_TYPE=${BOOK_TYPE})

set(RINGBUFFER_CAPACITY 4096)
add_compile_definitions(RINGBUFFER_CAPACITY=${RINGBUFFER_CAPACITY})

add_executable(matching_engine main.cpp
        src/book/types.h
        src/book/book.h
        src/book/order.h
        src/book/trade.h
        src/book/level.h
        src/book/shared_counter.h
        src/tcp/server.h
        src/tcp/connection.h
        src/common/header.h
        src/common/add_order.h
        src/common/types.h
        src/engine.h
)

add_executable(tcp_server server.cpp
        src/tcp/server.h
        src/tcp/connection.h
        src/common/header.h
        src/common/add_order.h
        src/common/types.h
)

add_executable(tcp_client client.cpp
        src/tcp/client.h
        src/common/header.h
        src/common/add_order.h
        src/common/types.h
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
        test/common/string_test.cpp
        test/common/header_test.cpp
        test/common/add_order_test.cpp
        test/book/book_level_test.cpp
)
target_link_libraries(matching_engine_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(matching_engine_test)