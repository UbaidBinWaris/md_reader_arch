cmake_minimum_required(VERSION 3.20)
project(NanoMark VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

enable_testing()

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    WebEngineWidgets
    PrintSupport
    Sql
)

# Collect all source files
set(SOURCES
    src/core/main.cpp
    src/core/Application.cpp
    src/core/SettingsService.cpp
    src/core/ServiceRegistry.cpp
    src/ui/MainWindow.cpp
    src/ui/TabManager.cpp
    src/ui/WindowManager.cpp
    src/ui/Dashboard.cpp
    src/editor/Editor.cpp
    src/editor/MarkdownHighlighter.cpp
    src/markdown/MarkdownRenderer.cpp
    src/preview/PreviewPane.cpp
    src/filemanager/FileManager.cpp
    src/filemanager/WorkspaceManager.cpp
    src/filemanager/AutosaveManager.cpp
    src/study/StudyMode.cpp
    src/study/StickyNotes.cpp
    src/study/Annotations.cpp
    src/study/NanomarkFile.cpp
    src/themes/ThemeManager.cpp
    src/export/ExportManager.cpp
    src/export/PDFExporter.cpp
    src/export/HTMLExporter.cpp
    src/utils/ErrorHandler.cpp
    src/utils/Logger.cpp
)

set(HEADERS
    src/core/Application.h
    src/core/SettingsService.h
    src/core/ServiceRegistry.h
    src/core/PluginInterface.h
    src/ui/MainWindow.h
    src/ui/TabManager.h
    src/ui/WindowManager.h
    src/ui/Dashboard.h
    src/editor/Editor.h
    src/editor/MarkdownHighlighter.h
    src/markdown/MarkdownRenderer.h
    src/preview/PreviewPane.h
    src/filemanager/FileManager.h
    src/filemanager/WorkspaceManager.h
    src/filemanager/AutosaveManager.h
    src/study/StudyMode.h
    src/study/StickyNotes.h
    src/study/Annotations.h
    src/study/NanomarkFile.h
    src/themes/ThemeManager.h
    src/export/ExportManager.h
    src/export/PDFExporter.h
    src/export/HTMLExporter.h
    src/utils/ErrorHandler.h
    src/utils/Logger.h
)

# Qt resource file
set(RESOURCES
    resources/resources.qrc
)

# Create the executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/editor
    ${CMAKE_SOURCE_DIR}/src/markdown
    ${CMAKE_SOURCE_DIR}/src/preview
    ${CMAKE_SOURCE_DIR}/src/filemanager
    ${CMAKE_SOURCE_DIR}/src/study
    ${CMAKE_SOURCE_DIR}/src/themes
    ${CMAKE_SOURCE_DIR}/src/export
    ${CMAKE_SOURCE_DIR}/src/utils
)

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::PrintSupport
    Qt6::Sql
)

# Compiler warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
add_subdirectory(tests)
