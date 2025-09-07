set(SERVER_HEADERS
        include/matchingengine/engine.h
        include/matchingengine/orderbook/book.h
        include/matchingengine/orderbook/level.h
        include/matchingengine/orderbook/order.h
        include/matchingengine/orderbook/shared_counter.h
        include/matchingengine/orderbook/trade.h
)

set(SERVER_SOURCES
        src/matchingengine/engine.cpp
        src/matchingengine/orderbook/book.cpp
        src/matchingengine/orderbook/level.cpp
        src/matchingengine/orderbook/order.cpp
        src/matchingengine/orderbook/trade.cpp
)

add_library(matching_engine_lib
        STATIC ${SERVER_HEADERS} ${SERVER_SOURCES}
)

target_include_directories(matching_engine_lib
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(matching_engine_lib
        PUBLIC
        matching_engine_core
        ${Boost_LIBRARIES}
)

target_compile_definitions(matching_engine_lib
        PUBLIC
        BOOST_LOG_DYN_LINK
        RINGBUFFER_CAPACITY=${RINGBUFFER_CAPACITY}
        MAX_PAYLOAD_SIZE=${MAX_PAYLOAD_SIZE}
)

add_executable(matching_engine_server src/matchingengine/main.cpp)
target_link_libraries(matching_engine_server PRIVATE matching_engine_lib)

# -------------------------------------------------------------

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/**/*.cpp)

if (TEST_SOURCES)
    add_executable(matching_engine_server_tests
            ${TEST_SOURCES}
    )

    target_link_libraries(matching_engine_server_tests
            PRIVATE matching_engine_lib matching_engine_test_helpers GTest::gtest_main
    )

    gtest_discover_tests(matching_engine_server_tests)

    add_test(NAME matching_engine_server_tests COMMAND matching_engine_server_tests)

    set_tests_properties(matching_engine_server_tests PROPERTIES TIMEOUT 30)
endif ()
