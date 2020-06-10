#include "Slider.h"

#include <imgui.h>
namespace imgui
{
    Slider::Slider(std::string name, float *value)
        : m_name(name), m_value(value) {}


    void Slider::compose()
    {
        //ImGui::SliderFloat(m_name, m_value, 1.0f, -1.0f);
    }
}
