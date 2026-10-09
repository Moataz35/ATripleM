cmake_minimum_required(VERSION 3.19)
project(Image_Nightmare LANGUAGES CXX)

# Find Qt6 components
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

# Enables AUTOMOC, AUTOUIC, AUTORCC, and standard C++ options
qt_standard_project_setup()

# Modern target-based include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

# Collect core C++ source files from src/
file(GLOB CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Define executable with Qt sources and Core sources
qt_add_executable(Image_Nightmare
    WIN32 MACOSX_BUNDLE
    
    # GUI Sources (inside GUI/)
    GUI/main.cpp
    GUI/MainWindow.cpp
    GUI/MainWindow.h
    GUI/MainWindow.ui
    GUI/navbar.h
    GUI/navbar.cpp
    GUI/EditorDropArea.h
    GUI/EditorDropArea.cpp
    GUI/CropBox.h

    # Core Logic Sources (from src/)
    ${CORE_SOURCES}
)

# Link Qt libraries
target_link_libraries(Image_Nightmare
    PRIVATE
        Qt::Core
        Qt::Widgets
)

# Installation setup
include(GNUInstallDirs)

install(TARGETS Image_Nightmare
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Qt deployment script generation
qt_generate_deploy_app_script(
    TARGET Image_Nightmare
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})