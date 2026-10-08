cmake_minimum_required(VERSION 3.19)
project(Image_Editor_GUI LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()
include_directories("../include/ " "../lib/include/ " )
aux_source_directory ("../src/" SOURCE_FILES)
qt_add_executable(Image_Editor_GUI
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${SOURCE_FILES}
)

target_link_libraries(Image_Editor_GUI
    PRIVATE
        Qt::Core
        Qt::Widgets
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
