cmake_minimum_required(VERSION 3.20)

find_package(Qt6 REQUIRED COMPONENTS Test Sql Svg)

add_executable(test_renderer test_renderer.cpp)

target_link_libraries(test_renderer PRIVATE
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Svg
)

target_include_directories(test_renderer PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/markdown
    ${CMAKE_SOURCE_DIR}/src/themes
    ${CMAKE_SOURCE_DIR}/src/utils
)

target_sources(test_renderer PRIVATE
    ${CMAKE_SOURCE_DIR}/src/markdown/MarkdownRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/themes/ThemeManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SettingsService.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/Logger.cpp
)

add_test(NAME test_renderer COMMAND test_renderer)
