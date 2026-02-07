cmake_minimum_required(VERSION 3.25)
project(matching_engine
        VERSION 1.0.0
        LANGUAGES CXX
)

# -------------------------------------------------------------
# Project-wide settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------------------------------------------
# Project-wide definitions
set(RINGBUFFER_CAPACITY 4096 CACHE STRING "Capacity of the Ring Buffer between the server and matching engine")
set(MAX_PAYLOAD_SIZE 256 CACHE STRING "Max payload size of the Ring Buffer between the server and matching engine")

# -------------------------------------------------------------
# Build type configuration
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

# Output directories (set BEFORE add_executable/add_library)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()
include(CTest)

# Compiler warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif ()

# -------------------------------------------------------------
# Dependencies
find_package(Boost REQUIRED COMPONENTS log log_setup system thread)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

include(FetchContent)

# GoogleTest dependency
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # for MSVC
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# -------------------------------------------------------------
# Add subdirectories
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(server)
add_subdirectory(client)

# -------------------------------------------------------------
# Installation rules
install(
        TARGETS matching_engine_core matching_engine_gui matching_engine_server
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# Install public headers
install(DIRECTORY core/include/ DESTINATION include/matching_engine/core)
install(DIRECTORY gui/include/ DESTINATION include/matching_engine/gui)
install(DIRECTORY server/include/ DESTINATION include/matching_engine/server)
