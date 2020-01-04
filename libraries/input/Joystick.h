#pragma once

#include <GLFW/glfw3.h>

class Joystick
{
public:
    Joystick(int joystickIndex);
    ~Joystick();

    void update();

    int getButton(int buttonChannel) const;
    float getAxis(int axisChannel) const;

private:
    int m_joystickIndex { -1 };
    GLFWgamepadstate m_gamepadState;
};
