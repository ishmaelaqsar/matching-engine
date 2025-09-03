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

# -------------------------------------------------------------
# Collect sources

set(CORE_HEADERS
        include/core/types.h
        include/core/ring_buffer.h
        include/core/protocol/header.h
        include/core/protocol/serialize_helper.h
        include/core/protocol/message.h
        include/core/protocol/view/get_book.h
        include/core/protocol/trading/add_order.h
        include/core/protocol/trading/modify_order.h
        include/core/protocol/trading/trade.h
)
set(ORDER_BOOK_HEADERS
        include/orderbook/book.h
        include/orderbook/engine.h
        include/orderbook/level.h
        include/orderbook/order.h
        include/orderbook/shared_counter.h
        include/orderbook/trade.h
)
set(TCP_HEADERS
        include/tcp/client.h
        include/tcp/connection.h
        include/tcp/server.h
)

set(CORE_SOURCES
        src/core/protocol/message.cpp
        src/core/protocol/trading/add_order.cpp
        src/core/protocol/trading/modify_order.cpp
        src/core/protocol/trading/trade.cpp
        src/core/protocol/view/get_book.cpp
        src/core/protocol/header.cpp
)
set(ORDER_BOOK_SOURCES
        src/orderbook/book.cpp
        src/orderbook/order.cpp
        src/orderbook/trade.cpp
        src/orderbook/engine.cpp
)
set(TCP_SOURCES
        src/tcp/client.cpp
        src/tcp/connection.cpp
        src/tcp/server.cpp
)

file(GLOB_RECURSE TEST_SOURCES test/*)

# -------------------------------------------------------------
# Core library
add_library(matching_engine_lib
        ${CORE_HEADERS}
        ${ORDER_BOOK_HEADERS}
        ${TCP_HEADERS}
        ${CORE_SOURCES}
        ${ORDER_BOOK_SOURCES}
        ${TCP_SOURCES}
)

target_include_directories(matching_engine_lib
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Dependencies
find_package(Boost REQUIRED COMPONENTS log log_setup system thread)

target_link_libraries(matching_engine_lib
        PUBLIC
        ${Boost_LIBRARIES}
)

target_compile_definitions(matching_engine_lib
        PUBLIC BOOST_LOG_DYN_LINK
)

# Compiler warnings
if (MSVC)
    target_compile_options(matching_engine_lib PRIVATE /W4 /permissive-)
else ()
    target_compile_options(matching_engine_lib PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
    )
endif ()

# -------------------------------------------------------------
# Main executables
add_executable(matching_engine server_main.cpp)
target_link_libraries(matching_engine PRIVATE matching_engine_lib)

add_executable(matching_engine_client client_main.cpp)
target_link_libraries(matching_engine_client PUBLIC matching_engine_lib)

# -------------------------------------------------------------
# Testing setup
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(matching_engine_test
        ${TEST_SOURCES}
)
target_link_libraries(matching_engine_test PRIVATE
        matching_engine_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(matching_engine_test)

# -------------------------------------------------------------
# Build type configuration
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

# Optional: Add debug symbols in Release mode
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (NOT MSVC)
        target_compile_options(matching_engine_lib PRIVATE -g)
    endif ()
endif ()

# -------------------------------------------------------------
# Installation rules
install(TARGETS matching_engine matching_engine_client
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/
        DESTINATION include/matching_engine
        FILES_MATCHING PATTERN "*.h"
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/matching_engine-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

# Optional: CTest configuration
if (BUILD_TESTING)
    set_tests_properties(matching_engine_test PROPERTIES
            TIMEOUT 30
    )
endif ()
