cmake_minimum_required(VERSION 3.19)
project(Image_Editor_GUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pixel loops in the filters are very slow without optimisation.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets Concurrent)

qt_standard_project_setup()

set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

qt_add_executable(Image_Editor_GUI
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    CanvasView.cpp
    CanvasView.h
    Widgets.cpp
    Widgets.h
    History.cpp
    History.h
    ImageConvert.cpp
    ImageConvert.h
    ImageOps.cpp
    ImageOps.h
    resources/resources.qrc
    ${ENGINE_DIR}/src/ImageEditor.cpp
    ${ENGINE_DIR}/src/ImageFilter.cpp
    ${ENGINE_DIR}/include/Image.h
    ${ENGINE_DIR}/include/ImageEditor.h
    ${ENGINE_DIR}/include/ImageFilter.h
)

target_include_directories(Image_Editor_GUI PRIVATE
    ${ENGINE_DIR}/include
    ${ENGINE_DIR}/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Image_Editor_GUI
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Concurrent
)

install(TARGETS Image_Editor_GUI
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Image_Editor_GUI
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
