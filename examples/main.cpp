#include <iostream>
#include <array>

#include <Engine.h>
#include <Window.h>
#include <Input.h>


float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    Window window(500, 500, "Vechicl Simulation");

    if (!window.createWindow())
    {
        std::cout << "Failed to create window" << "\n";
        return -1;
    }

    Input input;
    while (!window.shouldClose())
    {
        input.pollInput();
        window.simpleUpdate();
    }
    return 0;
}
