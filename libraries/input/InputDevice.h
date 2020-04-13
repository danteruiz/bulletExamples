#pragma once

#include <memory>
class InputDevice
{
public:
    using Pointer = std::shared_ptr<InputDevice>;
    enum Type
    {
        JOYSTICK,
        KEYBOARD,
        MOUSE
    };
    InputDevice(Type type) noexcept : m_type(type) {}

    Type getType() const { return m_type; }
    virtual void update() = 0;

protected:
    Type m_type;
};
