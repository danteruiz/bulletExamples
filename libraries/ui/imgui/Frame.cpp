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

        char* s = new char[100];
        size_t size = 100;
        std::cout << "collapsed: " << ImGui::CollapsingHeader("header") << std::endl;
        ImGui::Text("Insider header");

        std::cout << active << active2 << std::endl;

        std::cout << std::endl;
        ImGui::End();
    }
}
