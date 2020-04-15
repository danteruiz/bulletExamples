project(render)
set(TARGET_NAME render)
file(GLOB graphics_headers "*.h")
file(GLOB graphics_source "*.cpp")

add_library(render STATIC ${graphics_source} ${graphics_headers})

link_personal_libraries(glfw)
find_package(OPENGL REQUIRED)

target_link_libraries(render
  PUBLIC CONAN_PKG::glm OpenGL::GL CONAN_PKG::glew CONAN_PKG::glfw)

target_include_directories (render
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
