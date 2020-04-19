file(GLOB glfw_headers "*.h")
file(GLOB glfw_source "*.cpp")

add_library(core STATIC ${glfw_source} ${glfw_headers})

target_link_libraries(core
  PRIVATE CONAN_PKG::glfw)
