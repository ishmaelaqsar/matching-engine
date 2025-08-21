cmake_minimum_required(VERSION 3.25)
project(matching_engine
        VERSION 1.0.0
        LANGUAGES CXX
)

# Project-wide settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuration
set(RINGBUFFER_CAPACITY 4096 CACHE STRING "Ring buffer capacity")
add_compile_definitions(RINGBUFFER_CAPACITY=${RINGBUFFER_CAPACITY})

# Common source files
set(COMMON_HEADERS
        src/book/book.h
        src/book/order.h
        src/book/trade.h
        src/book/level.h
        src/book/shared_counter.h
        src/tcp/server.h
        src/tcp/connection.h
        src/tcp/header.h
        src/common/protocol/trading/add_order.h
        src/common/types.h
        src/common/protocol/ring_buffer.h
        src/common/util.h
        src/common/protocol/view/get_book.h
        src/tcp/handlers/trading.h
)

set(TCP_HEADERS
        src/tcp/server.h
        src/tcp/connection.h
        src/tcp/header.h
        src/tcp/client.h
        src/common/protocol/trading/add_order.h
        src/common/types.h
        src/common/util.h
        src/common/protocol/view/get_book.h
        src/tcp/handlers/trading.h
)

# Create a library for shared code
add_library(matching_engine_lib INTERFACE)
target_include_directories(matching_engine_lib INTERFACE src/)

# Main executables
add_executable(matching_engine
        main.cpp
        ${COMMON_HEADERS}
)
target_link_libraries(matching_engine PRIVATE matching_engine_lib)

add_executable(tcp_server
        server.cpp
        ${TCP_HEADERS}
)
target_link_libraries(tcp_server PRIVATE matching_engine_lib)

add_executable(tcp_client
        client.cpp
        src/tcp/client.h
        src/tcp/header.h
        src/common/protocol/trading/add_order.h
        src/common/types.h
        src/common/util.h
        src/common/protocol/view/get_book.h
)
target_link_libraries(tcp_client PRIVATE matching_engine_lib)

# Dependencies
find_package(Boost REQUIRED COMPONENTS log log_setup system thread)

# Testing setup
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test executable
add_executable(matching_engine_test
        test/tcp/header_test.cpp
        test/common/string_test.cpp
        test/common/protocol/trading/add_order_test.cpp
        test/book/level_test.cpp
        test/book/book_test.cpp
        test/helpers.h
        src/common/protocol/view/get_book.h
        test/common/protocol/view/get_book_test.cpp
)

target_link_libraries(matching_engine_test PRIVATE
        matching_engine_lib
        GTest::gtest_main
        ${Boost_LIBRARIES}  # Using legacy variables for compatibility
)

target_include_directories(matching_engine_test PRIVATE
        ${Boost_INCLUDE_DIRS}
)

target_compile_definitions(matching_engine_test PRIVATE
        BOOST_LOG_DYN_LINK
)

include(GoogleTest)
gtest_discover_tests(matching_engine_test)

# Optional: Add compiler warnings for better code quality
if(MSVC)
    target_compile_options(matching_engine_lib INTERFACE /W4)
else()
    target_compile_options(matching_engine_lib INTERFACE
            -Wall -Wextra -Wpedantic
    )
endif()

# Optional: Installation rules
install(TARGETS matching_engine tcp_server tcp_client
        RUNTIME DESTINATION bin
)
