project(rendering-examples)
cmake_minimum_required(VERSION 3.11)

include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/macros/LinkPersonalLibraries.cmake)
conan_add_remote(NAME bincrafters INDEX 1
  URL https://api.bintray.com/conan/bincrafters/public-conan)
conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP CMAKE_TARGETS
  BUILD missing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)
if (UNIX)
  add_compile_options(-W -Wall -Wextra -Wpedantic -fvisibility=hidden -std=c++17)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")


set(LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/libraries)
add_subdirectory(${CMAKE_SOURCE_DIR}/libraries)
add_subdirectory(${CMAKE_SOURCE_DIR}/examples)
