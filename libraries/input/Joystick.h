#pragma once

#include <GLFW/glfw3.h>
#include "InputDevice.h"

class Joystick : public InputDevice
{
public:
    Joystick(int joystickIndex, InputDevice::Type type) noexcept;

    void update() override;

    int getButton(int buttonChannel) const;
    float getAxis(int axisChannel) const;

private:
    int m_joystickIndex { -1 };
    GLFWgamepadstate m_gamepadState;
};
