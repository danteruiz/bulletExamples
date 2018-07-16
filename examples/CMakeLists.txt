###
project(test)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(test render physics)
