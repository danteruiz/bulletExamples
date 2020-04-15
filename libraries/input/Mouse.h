#pragma once

#include <tuple>
#include <array>

#include "InputDevice.h"


static const int MAX_MOUSE_BUTTONS { 9 };

struct GLFWwindow;
class Mouse : public InputDevice
{
public:
    enum MouseAxisChannel
    {
        MOUSE_AXIS_Y,
        MOUSE_AXIS_X,
        MOUSE_AXIS_X_DELTA,
        MOUSE_AXIS_Y_DELTA,
        MOUSE_AXIS_NUM
    };

    Mouse(InputDevice::Type type) noexcept;

    int getButton(int channel) { return m_buttonStates[channel]; }
    float getAxis(int channel) { return m_axisStates[channel]; }

    void update() override;
    void mouseUpdate(GLFWwindow* window);

private:
    struct Cursor
    {
        double x { 0.0 };
        double y { 0.0 };
    };

    Cursor m_lastCursor;
    std::array<int, MAX_MOUSE_BUTTONS> m_buttonStates;
    std::array<float, MOUSE_AXIS_NUM> m_axisStates;
};
