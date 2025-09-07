set(CLIENT_HEADERS
        include/matchingengine/parser.h
)

add_executable(matching_engine_client src/matchingengine/main.cpp
        ${CLIENT_HEADERS}
)

target_include_directories(matching_engine_client
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(matching_engine_client PRIVATE matching_engine_core)