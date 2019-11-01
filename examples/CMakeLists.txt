project(demo)

if (MSVC)
  add_executable(${PROJECT_NAME} WIN32main.cpp)
else()
  add_executable(${PROJECT_NAME} main.cpp)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE render physics)

