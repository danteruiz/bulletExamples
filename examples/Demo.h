#pragma once

#include "GlfwApplication.h"


class DemoApplication : public GlfwApplication
{
public:
    DemoApplication();

    int type;

    void exec() override {}

private:
};
