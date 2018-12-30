# cmake file for rendering
project(render)
file(GLOB graphics_headers "*.h")
file(GLOB graphics_source "*.cpp")

add_library(render STATIC ${graphics_source} ${graphics_headers})
find_package(OPENGL REQUIRED)

target_link_libraries(render
  PUBLIC ${CONAN_LIBS_GLM}
  PRIVATE ${CONAN_LIBS_GLFW} ${OPENGL_LIBRARY} ${glfw})

target_include_directories (render
  PUBLIC ${CONAN_INCLUDE_DIRS_GLM} ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CONAN_INCLUDE_DIRS_GLFW} ${OPENGL_INCLUDE_DIR} ${glfw})
