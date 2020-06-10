#include "Button.h"

#include <imgui.h>
namespace imgui
{
    Button::Button(std::string name, std::function<void(bool)> function) :
        m_name(name), m_function(function) {}


    void Button::compose()
    {
        bool clicked = ImGui::Button(m_name.c_str());

        if (m_clicked != clicked)
        {
            m_function(clicked);
        }

        m_clicked = clicked;
    }
}
