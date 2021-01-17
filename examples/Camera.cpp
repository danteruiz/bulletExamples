#include "Camera.h"

#include <Input.h>


Camera::Camera(Input* input) : m_input(input) {}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::mat4();
}

glm::mat4 Camera::getViewPosition() const
{
    return glm::vec3();
}
