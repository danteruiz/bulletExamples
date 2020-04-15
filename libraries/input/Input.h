#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "InputDevice.h"


class Mouse;
class Input
{
public:
    Input();
    ~Input() = default;

    void pollInput();
private:
    std::unordered_map<int, InputDevice::Pointer> m_inputDevices;
};
