file(GLOB glfw_headers "*.h")
file(GLOB glfw_source "*.cpp")
set(TARGET_NAME core)

add_library(${TARGET_NAME} STATIC ${glfw_source} ${glfw_headers})
target_link_libraries(${TARGET_NAME}
  PRIVATE glfw::glfw glew::glew
  PUBLIC fmt::fmt spdlog::spdlog spdlog::libspdlog)

message(spdlog::spdlog)
