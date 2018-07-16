find_package(BULLET REQUIRED)
file(GLOB physics_sources "*.cpp" "*.h")
add_library(physics "")

target_sources(physics PRIVATE ${physics_sources})

target_link_libraries(physics
  PUBLIC ${GLM_LIBRARAY}
  PRIVATE ${BULET_LIBRARY})

target_include_directories(physics
  PUBLIC ${GLM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${BULLET_INCLUDE_DIRS})
