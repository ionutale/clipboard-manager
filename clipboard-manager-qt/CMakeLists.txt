cmake_minimum_required(VERSION 3.16)
project(ClipboardManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Sql)

add_executable(clipboard-manager-qt
    resources.qrc
    src/main.cpp
    src/clipboard/ClipboardMonitor.cpp
    src/database/Database.cpp
    src/ui/PopupWidget.cpp
    src/ui/HistoryTab.cpp
    src/ui/NotesTab.cpp
)

target_link_libraries(clipboard-manager-qt PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

target_include_directories(clipboard-manager-qt PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)
