#include "Frame.h"


#include <imgui.h>
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

#include <iostream>

#include <array>
#include <vector>
namespace imgui
{
    Frame::Frame(std::string name) : m_name(name) {}

    void Frame::compose()
    {
        ImGui::Begin(m_name.c_str());

        for (auto widget: m_widgets)
        {
            widget->compose();
        }

        ImGui::End();
    }


    TestFrame::TestFrame(std::string name) : Frame(name) {}

    void TestFrame::compose()
    {
        ImGui::Begin("TestFrame");

        ImGui::SliderFloat3(" Postion", m_f3, 0.0f, 1.0f);
        ImGui::ColorPicker3("colors", colors);
        std::cout << m_f3[0] << std::endl;
        std::cout << m_f3[1] << std::endl;
        std::cout << m_f3[2] << std::endl;

        bool active = true;
        bool active2 = false;
        ImGui::RadioButton("Radio Button", active);
        ImGui::RadioButton("Radio Button", active2);

        std::vector<std::string> elements = { "One", "Two", "Three" };

        static auto vector_getter = [](void* vec, int idx, const char** out_text)
        {
            auto& vector = *static_cast<std::vector<std::string>*>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = vector.at(idx).c_str();
            return true;
        };

        //ImGui::Combo("combo", &index, 
        ImGui::ListBox("List Box: ",&index, vector_getter, static_cast<void*>(&elements), elements.size(), 4);

        char* s = new char[100];
        size_t size = 100;
        ImGui::InputText("Input Text", s, size);
        std::cout << "collapsed: " << ImGui::CollapsingHeader("header") << std::endl;
        ImGui::Text("Insider header");

        std::cout << active << active2 << std::endl;

        std::cout << std::endl;
        ImGui::End();
    }
}
