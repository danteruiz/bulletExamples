project(demo)
set(TARGET_NAME demo)
file(GLOB source_files "*.h" "*.cpp")
if (MSVC)
  add_executable(${PROJECT_NAME} ${source_files})
else()
  add_executable(${PROJECT_NAME} main.cpp)
endif()

link_personal_libraries(render input core)

target_link_libraries(${TARGET_NAME} PRIVATE CONAN_PKG::imgui)
