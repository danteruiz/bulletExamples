#pragma once

#include <vector>
#include <memory>

#include "InputDevice.h"
class Input
{
public:
    Input();
    ~Input() = default;

    void pollInput();
private:
    std::vector<InputDevice::Pointer> m_inputDevices;
};
