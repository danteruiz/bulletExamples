#include <iostream>
#include <string_view>
#include <Engine.h>
#include <Window.h>

int main() {

    Window window(500, 500, "This is just a test");

    if (!window.createWindow())
    {
        std::cout << "Failed to create window" << "\n";
        return -1;
    }

    while (!window.shouldClose())
    {
        window.simpleUpdate();
    }
    return 0;
}
