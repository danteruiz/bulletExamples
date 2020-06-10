#pragma once

#include <vector>

struct GLFWwindow;
namespace imgui
{
    class Frame;

    class Imgui
    {
    public:
        Imgui(GLFWwindow *window);

        void addFrame(imgui::Frame *frame);
        void composeFrames();
        void render();
    private:
        std::vector<imgui::Frame*> m_frames;
    };
}
