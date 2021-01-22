project(rendering-examples)
cmake_minimum_required(VERSION 3.11)

set(GLTF_MODEL_PATH "${CMAKE_SOURCE_DIR}/resources/glTF-models/")
set(IBL_PATH "${CMAKE_SOURCE_DIR}/resources/images/IBL/")

message("GLTF_MODEL_PATH=${GLTF_MODEL_PATH}")
include(${CMAKE_SOURCE_DIR}/cmake/conan.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/macros/LinkPersonalLibraries.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/macros/GenerateModelPaths.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/macros/GenerateIBLPaths.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)
if (UNIX)
  add_compile_options(-W -Wall -Wextra -Wpedantic -fvisibility=hidden -std=c++20)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/engine)
add_subdirectory(${CMAKE_SOURCE_DIR}/engine)
add_subdirectory(${CMAKE_SOURCE_DIR}/examples)
