cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_STANDARD 17)

include_directories("include/" "lib/include/")

aux_source_directory("src/" SOURCE_FILES)

project(ImageNightmare)

add_executable(
    ImageNightmare
    ${SOURCE_FILES}
)
