#include "Mouse.h"

#include <GLFW/glfw3.h>

Mouse::Mouse(InputDevice::Type type) noexcept: InputDevice(type) {}


void Mouse::update()
{

    int numButtons = (int) m_buttonStates.size();
    for (int buttonIndex = 0; buttonIndex < numButtons; buttonIndex++)
    {
        //m_buttonStates[buttonIndex] = glfwGetMouseButton(window, buttonIndex);
    }
}


void Mouse::mouseUpdate(GLFWwindow* window)
{
    int numButtons = (int) m_buttonStates.size();
    for (int buttonIndex = 0; buttonIndex < numButtons; buttonIndex++)
    {
        m_buttonStates[buttonIndex] = glfwGetMouseButton(window, buttonIndex);
    }


    Mouse::Cursor cursor;
    glfwGetCursorPos(window, &cursor.x, &cursor.y);

    m_axisStates[MOUSE_AXIS_Y_DELTA] = (float) (cursor.y - m_lastCursor.y);
    m_axisStates[MOUSE_AXIS_X_DELTA] = (float) (cursor.x - m_lastCursor.x);
    m_axisStates[MOUSE_AXIS_Y] = (float) cursor.y;
    m_axisStates[MOUSE_AXIS_X] = (float) cursor.x;

    m_lastCursor = cursor;

}
