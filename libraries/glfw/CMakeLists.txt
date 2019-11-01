file(GLOB glfw_headers "*.h")
file(GLOB glfw_source "*.cpp")

add_library(glfw STATIC ${glfw_source} ${glfw_headers})

target_link_libraries(glfw
  PUBLIC CONAN_PKG::glfw)
