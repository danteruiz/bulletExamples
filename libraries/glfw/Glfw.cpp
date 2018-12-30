#include "Glfw.h"

namespace glfw
{

    bool initialized = false;
    bool isInitialized() { return initialized; }
    void initialize()
    {

        initialized = true;
    }

    void terminate()
    {
        initialized = false;
    }
}
