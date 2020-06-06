#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <chrono>

#include "wrappers/glfw.hpp"
#include "scene/Scene.hpp"
#include "scene/ImGui.hpp"
#include "resources/FileUtils.hpp"


using HighClock = std::chrono::high_resolution_clock;

void enableOpenGLErrorHandler();

int main()
{
    //std::string dummy;
    //std::getline(std::cin, dummy);

    try
    {
        glfw::InitGuard initGuard;

        glfw::WindowHint hint;
        hint.contextVersion(3, 3, glfw::Profile::Core);
        hint.resizable(false);
        hint.depthBits(32);
        hint.stencilBits(0);
        hint.doublebuffer();
        hint.debugContext();

        glfw::Window window(1280, 720, "INF443 Project", hint);
        window.makeCurrent();
        //window.setSwapInterval(1);

        std::cout << window.getContextVersion().toString() << std::endl;

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Failed to initialize GLAD!");

        // Setup Dear Imgui
        scene::ImGuiGuard imGuiGuard(window);

#ifndef NDEBUG
        enableOpenGLErrorHandler();
#endif
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        file_utils::addDefaultLoaders();
        scene::Scene scene(window);

        auto then = HighClock::now();
        while (!window.shouldClose())
        {
            auto now = HighClock::now();
            auto delta = (now - then).count() / 1000000000.0;
            delta = std::min(delta, 1.0 / 30.0);

            scene::beginImGui();
            scene.update(delta);
            scene.draw(delta);
            scene::endImGui();

            window.swapBuffers();
            glfw::pollEvents();

            then = now;
        }
    }
    catch (std::exception &exc)
    {
        std::cout << typeid(exc).name() << ": " << exc.what() << std::endl;
    }
}