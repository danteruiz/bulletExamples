project(render)
file(GLOB graphics_headers "*.h")
file(GLOB graphics_source "*.cpp")

add_library(render STATIC ${graphics_source} ${graphics_headers})
find_package(OPENGL REQUIRED)

target_link_libraries(render
  PUBLIC CONAN_PKG::glm
  PRIVATE OpenGL::GL ${glfw} CONAN_PKG::glfw)

target_include_directories (render
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${LIBRARIES_DIR}/glfw)
