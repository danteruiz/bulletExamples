project(input)

file(GLOB input_sources "*.h" "*.cpp")
add_library(${PROJECT_NAME} STATIC ${input_sources})

target_link_libraries(${PROJECT_NAME}
  PRIVATE CONAN_PKG::glfw)

target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
