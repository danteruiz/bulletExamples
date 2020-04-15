project(demo)

file(GLOB source_files "*.h" "*.cpp")
if (MSVC)
  add_executable(${PROJECT_NAME} ${source_files})
else()
  add_executable(${PROJECT_NAME} main.cpp)
endif()

target_link_libraries(${PROJECT_NAME} render input)
