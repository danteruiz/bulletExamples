#
cmake_minimum_required(VERSION 3.11)

add_compile_options(-W -Wall  -Wextra -Wpedantic -Werror -std=c++1z)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

add_subdirectory(${CMAKE_SOURCE_DIR}/libraries)
add_subdirectory(${CMAKE_SOURCE_DIR}/examples)
