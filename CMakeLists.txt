#
cmake_minimum_required(VERSION 3.11)

add_definitions("-std=c++17")
add_compile_options(-W -Wall  -Wextra -Wpedantic -Werror -std=c++1z)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


add_subdirectory(${CMAKE_SOURCE_DIR}/libraries)
add_subdirectory(${CMAKE_SOURCE_DIR}/examples)
