#include "Input.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include "InputChannels.h"
#include "Joystick.h"

Joystick* joystick;


void joystickConnectioncallback(int jid, int event)
{
        switch (event)
        {
            case GLFW_CONNECTED:
                std::cout << "conencted: " << jid << std::endl;
                break;


            case GLFW_DISCONNECTED:
                std::cout << "disconnected: " << jid << std::endl;
            default:
                break;
        };
}

Input::Input()
{
    const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
    std::cout << name << std::endl;
    glfwSetJoystickCallback(joystickConnectioncallback);
    joystick = new Joystick(GLFW_JOYSTICK_1);
}

bool isButtonPressed(const unsigned char button)
{
    return (button == GLFW_PRESS);
}

void Input::pollInput()
{
    joystick->update();
    /*std::cout << joystick->getButton(input::A) << "\n";
    std::cout << joystick->getButton(input::B) << "\n";
    std::cout << joystick->getButton(input::Y) << "\n";
    std::cout << joystick->getButton(input::X) << "\n";
    std::cout << joystick->getButton(input::LB) << "\n";
    std::cout << joystick->getButton(input::RB) << "\n";
    std::cout << joystick->getButton(input::SELECT) << "\n";
    std::cout << joystick->getButton(input::START) << "\n";
    std::cout << joystick->getButton(input::HOME) << "\n";
    std::cout << joystick->getButton(input::LS) << "\n";
    std::cout << joystick->getButton(input::RS) << "\n";
    std::cout << joystick->getButton(input::DPAD_UP) << "\n";
    std::cout << joystick->getButton(input::DPAD_RIGHT) << "\n";

    std::cout << "\n \n";*/
}
