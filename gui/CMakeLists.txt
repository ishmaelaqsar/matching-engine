set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(GUI_SOURCES
        src/main.cpp
        src/mainwindow.cpp
)

add_executable(matching_engine_gui
        ${GUI_SOURCES}
)

set_target_properties(matching_engine_gui
        PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_include_directories(matching_engine_gui
        PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(matching_engine_gui
        PRIVATE
        matching_engine_core
        Qt6::Widgets
)
