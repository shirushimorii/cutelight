cmake_minimum_required(VERSION 3.25)

project(CuteLight
    VERSION 0.1.0
    DESCRIPTION "macOS Sequoia / Tahoe inspired Spotlight for Cutefish"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CutefishToolkit REQUIRED)

add_executable(cutelight
    src/main.cpp
    src/application/Application.cpp
    src/integration/ControlService.cpp
    src/integration/MainMenu.cpp
    src/integration/TrayIcon.cpp
    src/settings/Settings.cpp
    src/settings/Autostart.cpp
    src/search/SearchController.cpp
    src/window/MainWindow.cpp
    src/window/SettingsWindow.cpp
    src/ui/CuteLightView.cpp
    src/ui/SearchBox.cpp
    src/ui/ResultsView.cpp
    src/ui/ClearButton.cpp
    src/ui/ShortcutButton.cpp
    src/ui/SettingsView.cpp
)

target_include_directories(cutelight PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(cutelight PRIVATE Cutefish::Toolkit)

target_compile_options(cutelight PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS cutelight RUNTIME DESTINATION bin)
