cmake_minimum_required(VERSION 3.16)
project(ATripleM)

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
    MainWindow.ui
    resources.qrc
    ${SOURCE_FILES}
)

add_executable(GUI ${PROJECT_SOURCES})

if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
    file(GENERATE
        OUTPUT "${APP_ICON_RESOURCE}"
        CONTENT "#define IDI_APP_ICON 1\nIDI_APP_ICON ICON \"${CMAKE_CURRENT_SOURCE_DIR}/icons/app_icon.ico\"\n"
    )
    set_source_files_properties("${APP_ICON_RESOURCE}" PROPERTIES GENERATED TRUE LANGUAGE RC)
    target_sources(GUI PRIVATE "${APP_ICON_RESOURCE}")
endif()

target_link_libraries(GUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(GUI PROPERTIES
    WIN32_EXECUTABLE FALSE
)
