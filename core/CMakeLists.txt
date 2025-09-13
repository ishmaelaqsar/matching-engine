set(CORE_HEADERS
        include/matchingengine/types.h
        include/matchingengine/ring_buffer.h
        include/matchingengine/protocol/header.h
        include/matchingengine/protocol/serialize_helper.h
        include/matchingengine/protocol/message.h
        include/matchingengine/protocol/admin/login.h
        include/matchingengine/protocol/info/info.h
        include/matchingengine/protocol/info/error.h
        include/matchingengine/protocol/view/get_book.h
        include/matchingengine/protocol/view/get_orders.h
        include/matchingengine/protocol/trading/add_order.h
        include/matchingengine/protocol/trading/modify_order.h
        include/matchingengine/protocol/trading/trade.h
        include/matchingengine/protocol/trading/cancel_order.h
        include/matchingengine/tcp/client.h
        include/matchingengine/tcp/connection.h
        include/matchingengine/tcp/server.h
        include/matchingengine/tcp/util.h
)

set(CORE_SOURCES
        lib/matchingengine/protocol/message.cpp
        lib/matchingengine/protocol/admin/login.cpp
        lib/matchingengine/protocol/info/info.cpp
        lib/matchingengine/protocol/info/error.cpp
        lib/matchingengine/protocol/trading/add_order.cpp
        lib/matchingengine/protocol/trading/modify_order.cpp
        lib/matchingengine/protocol/trading/trade.cpp
        lib/matchingengine/protocol/view/get_book.cpp
        lib/matchingengine/protocol/view/get_orders.cpp
        lib/matchingengine/protocol/header.cpp
        lib/matchingengine/protocol/trading/cancel_order.cpp
        lib/matchingengine/tcp/client.cpp
        lib/matchingengine/tcp/connection.cpp
        lib/matchingengine/tcp/server.cpp
)

add_library(matching_engine_core
        STATIC ${CORE_HEADERS} ${CORE_SOURCES}
)

target_include_directories(matching_engine_core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(matching_engine_core
        PUBLIC ${Boost_LIBRARIES}
)

target_compile_definitions(matching_engine_core
        PUBLIC
        BOOST_LOG_DYN_LINK
        RINGBUFFER_CAPACITY=${RINGBUFFER_CAPACITY}
        MAX_PAYLOAD_SIZE=${MAX_PAYLOAD_SIZE}
        CONNECTION_QUEUE_CAPACITY=${CONNECTION_QUEUE_CAPACITY}
)

# -------------------------------------------------------------

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/**/*.cpp)

if (TEST_SOURCES)
    file(GLOB_RECURSE TEST_HELPERS tests/**/*.h)

    add_library(matching_engine_test_helpers STATIC ${TEST_HELPERS})

    target_include_directories(matching_engine_test_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests)

    target_link_libraries(matching_engine_test_helpers PRIVATE matching_engine_core)

    add_executable(matching_engine_core_tests
            ${TEST_SOURCES}
    )

    target_link_libraries(matching_engine_core_tests
            PRIVATE matching_engine_core matching_engine_test_helpers GTest::gtest_main
    )

    gtest_discover_tests(matching_engine_core_tests)

    add_test(NAME matching_engine_core_tests COMMAND matching_engine_core_tests)

    set_tests_properties(matching_engine_core_tests PROPERTIES TIMEOUT 30)
endif ()