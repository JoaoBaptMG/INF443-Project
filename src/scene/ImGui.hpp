#pragma once

#include "wrappers/glfw.hpp"

namespace scene
{
    struct ImGuiGuard
    {
        ImGuiGuard(const glfw::Window& window);
        ~ImGuiGuard();
    };

    void beginImGui();
    void endImGui();
}
