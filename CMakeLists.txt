cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory("./" SOURCE_FILES)

project(ImageNightmare)

add_executable(
    ImageNightmare
    ${SOURCE_FILES}
)
