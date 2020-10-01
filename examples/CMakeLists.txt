project(demo)
set(TARGET_NAME demo)
file(GLOB source_files "*.h" "*.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/src/*.cpp" "${CMAKE_CURRENT_BINARY_DIR}/src/*.h")

message("${source_files}")
if (MSVC)
  add_executable(${PROJECT_NAME} ${source_files})
else()
  add_executable(${PROJECT_NAME} ${source_files})
endif()

generate_model_paths()

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCE_PATH="${CMAKE_SOURCE_DIR}/resources/")
link_personal_libraries(render input core ui)
target_include_directories(demo
  PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src)
