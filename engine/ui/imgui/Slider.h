#pragma once

#include "Widget.h"

#include <string>
namespace imgui
{
    class Slider : public Widget
    {
    public:
        Slider(std::string name, float *value);
        void compose() override;

    private:
        float *m_value;
        std::string m_name;
    };
}
