cmake_minimum_required(VERSION 3.16)
project(ImageNightmare)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories("../include" "../lib/include")
aux_source_directory("../src" SOURCE_FILES)
set(PROJECT_SOURCES
    main.cpp
    SideBar.cpp
    SideBar.h
    StatusMenu.cpp
    StatusMenu.h
    MainWindow.h
    MainWindow.cpp
    navbar.h
    navbar.cpp
    EditorDropArea.h
    EditorDropArea.cpp
    CropBox.h
    SideBar.ui
    resources.qrc
    ${SOURCE_FILES}
)

add_executable(GUI ${PROJECT_SOURCES})

target_link_libraries(GUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(GUI PROPERTIES
    WIN32_EXECUTABLE FALSE
)