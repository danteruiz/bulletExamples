#pragma once

#include "Widget.h"


#include <string>
#include <memory>
#include <vector>
namespace imgui
{
    class Frame : public Widget
    {
    public:
        Frame(std::string name);
        ~Frame();

        void addWidget(imgui::Widget* widget) { m_widgets.push_back(widget); }

        void compose() override;

    private:
        std::string m_name;
        std::vector<imgui::Widget*> m_widgets;
    };


    class TestFrame : public Frame
    {
    public:
        TestFrame(std::string name);
        void compose() override;

    private:
        float m_f3 [3];
        float colors[3];
        int index { 0 };
    };
}
