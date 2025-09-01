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

# Source file organization
set(COMMON_HEADERS
        src/common/types.h
        src/common/ring_buffer.h
        src/common/protocol/header.h
        src/common/protocol/serialize_helper.h
        src/common/protocol/message.h
        src/common/protocol/view/get_book.h
        src/common/protocol/trading/add_order.h
        src/common/protocol/trading/modify_order.h
        src/common/protocol/trading/trade.h
)

set(ORDER_BOOK_HEADERS
        src/book/book.h
        src/book/order.h
        src/book/trade.h
        src/book/level.h
        src/book/shared_counter.h
        src/book/engine.h
)

set(TCP_HEADERS
        src/tcp/server.h
        src/tcp/connection.h
        src/tcp/client.h
)

set(TEST_SOURCES
        test/book/level_test.cpp
        test/book/book_test.cpp
        test/helpers.h
        test/common/protocol/header_test.cpp
        test/common/protocol/view/get_book_test.cpp
        test/common/protocol/trading/add_order_test.cpp
        test/common/protocol/trading/modify_order_test.cpp
        src/book/engine.h
        src/common/protocol/trading/trade.h
)

# Create interface library for shared code
add_library(matching_engine_lib INTERFACE)
target_include_directories(matching_engine_lib INTERFACE src/)

# Dependencies
find_package(Boost REQUIRED COMPONENTS log log_setup system thread)

# Link Boost to the interface library so all targets get it
target_link_libraries(matching_engine_lib INTERFACE
        ${Boost_LIBRARIES}
)
target_include_directories(matching_engine_lib INTERFACE
        ${Boost_INCLUDE_DIRS}
)
target_compile_definitions(matching_engine_lib INTERFACE
        BOOST_LOG_DYN_LINK
)

# Compiler warnings
if(MSVC)
    target_compile_options(matching_engine_lib INTERFACE /W4 /permissive-)
else()
    target_compile_options(matching_engine_lib INTERFACE
            -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
    )
endif()

# Main executables
add_executable(matching_engine
        main.cpp
        ${COMMON_HEADERS}
        ${ORDER_BOOK_HEADERS}
)
target_link_libraries(matching_engine PRIVATE matching_engine_lib)

add_executable(tcp_client
        client.cpp
        ${COMMON_HEADERS}
        ${TCP_HEADERS}
)
target_link_libraries(tcp_client PRIVATE matching_engine_lib)

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
        ${COMMON_HEADERS}
        ${ORDER_BOOK_HEADERS}
        ${TEST_SOURCES}
)

target_link_libraries(matching_engine_test PRIVATE
        matching_engine_lib
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(matching_engine_test)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Optional: Add debug symbols in Release mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(NOT MSVC)
        target_compile_options(matching_engine_lib INTERFACE -g)
    endif()
endif()

# Installation rules
install(TARGETS matching_engine tcp_client
        RUNTIME DESTINATION bin
)

# Optional: Install headers for library usage
install(DIRECTORY src/
        DESTINATION include/matching_engine
        FILES_MATCHING PATTERN "*.h"
)

# Optional: Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/matching_engine-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

# Optional: CTest configuration
if(BUILD_TESTING)
    # Add custom test configurations here
    set_tests_properties(matching_engine_test PROPERTIES
            TIMEOUT 30
    )
endif()
