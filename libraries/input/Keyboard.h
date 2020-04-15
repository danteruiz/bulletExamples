#pragma once

#include <unordered_set>

#include "InputDevice.h"

static int const MAX_KEYBOARD_KEYS = 349;

struct GLFWwindow;
class Keyboard : public InputDevice
{
public:
    Keyboard(InputDevice::Type) noexcept;

    int getButton(int channel) {}

    void update() override {}
    void updateKeyboard(GLFWwindow* window) {}


private:
    std::unordered_set<int> m_buttons;
};
