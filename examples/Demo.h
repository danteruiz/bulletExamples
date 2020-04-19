#pragma once

#include "GlfwApplication.h"


class Window;

class DemoApplication : public GlfwApplication
{
public:
    DemoApplication();

    void exec() override;
};
