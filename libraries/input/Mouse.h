#pragma once

#include <tuple>
#include "InputDevice.h"



class Mouse : public InputDevice
{
public:

    struct Cursor
    {
        double x { 0.0 };
        double y { 0.0 };
    }
    Mouse(InputDevice::Type type) noexcept;

private:
    Cursor m_lastMousePosition;
};
