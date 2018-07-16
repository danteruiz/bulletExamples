# cmake file for rendering
project(render)
file(GLOB graphics_headers "*.h")
file(GLOB graphics_source "*.cpp")

add_library(render STATIC ${graphics_source} ${graphics_headers})
find_package(OPENGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLM REQUIRED)

target_link_libraries(render
  PUBLIC ${GLM_LIBRARY}
  PRIVATE ${GLFW3_LIBRARY} ${OPENGL_LIBRARY})

target_include_directories (render
  PUBLIC ${GLM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${GLFW3_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
