#pragma once

#include "GlfwApplication.h"
#include <memory>

class Window;
class Buffer;
class DemoApplication : public GlfwApplication
{
public:
    DemoApplication();

    void exec() override;

private:
    std::shared_ptr<Buffer> m_ebo;
    std::shared_ptr<Buffer> m_vbo;
    std::shared_ptr<Buffer> m_vbo2;
};
