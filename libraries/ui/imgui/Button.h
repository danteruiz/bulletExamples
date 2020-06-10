#pragma once

#include "Widget.h"

#include <string>
#include <functional>
namespace imgui
{
    class Button : public imgui::Widget
    {
    public:
        Button(std::string name, std::function<void(bool)> function);
        void compose() override;

    private:
        bool m_clicked { false };
        std::function<void(bool)> m_function;
        std::string m_name;
    };
}
