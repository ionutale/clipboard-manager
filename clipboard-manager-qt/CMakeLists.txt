cmake_minimum_required(VERSION 3.16)
project(ClipboardManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Qml Widgets Sql Test)

# Backend library — used by both app and tests
add_library(clipboardmanager-lib STATIC
    src/clipboard/ClipboardMonitor.cpp
    src/database/Database.cpp
    src/ui/TrayManager.cpp
    src/models/ClipboardListModel.cpp
)

target_link_libraries(clipboardmanager-lib PUBLIC
    Qt6::Quick
    Qt6::Qml
    Qt6::Widgets
    Qt6::Sql
)

target_include_directories(clipboardmanager-lib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Main executable
add_executable(clipboard-manager-qt
    resources.qrc
    src/main.cpp
)

target_link_libraries(clipboard-manager-qt PRIVATE
    clipboardmanager-lib
)

# Packaging
set(CPACK_PACKAGE_NAME "ClipboardManager")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "clipboard-manager")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A lightweight clipboard manager")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "clipboard-manager")
include(CPack)

# Tests
enable_testing()

function(add_qt_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE
        clipboardmanager-lib
        Qt6::Test
    )
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

add_qt_test(test_database tests/test_database.cpp)
add_qt_test(test_clipboard tests/test_clipboard.cpp)
