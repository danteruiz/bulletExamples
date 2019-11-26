#include "Input.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include "JoystickControls.h"


void callback(int jid, int event)
{
        switch (event)
        {
            case GLFW_CONNECTED:
                std::cout << "conencted: " << jid << std::endl;
                break;

            default:
                break;
        };
}

Input::Input()
{
    const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
    std::cout << name << std::endl;
    glfwSetJoystickCallback(callback);
}

void Input::pollInput()
{
    int count;
    const float* axis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);


    std::cout << "LX: " << axis[input::LX] << "\n";
    std::cout << "LY: " << axis[input::LY] << "\n";
    std::cout << "RX: " << axis[input::RX] << "\n";
    std::cout << "RB: " << axis[input::RB] << "\n";
    std::cout << "LB: " << axis[input::LB] << "\n";
    std::cout << "RY: " << axis[input::RY] << "\n";
    std::cout << "\n";
}
