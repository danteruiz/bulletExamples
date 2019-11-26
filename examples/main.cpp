#include <iostream>
#include <Engine.h>
#include <Window.h>
#include <Input.h>

int main(int argc, char* argv[])
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
