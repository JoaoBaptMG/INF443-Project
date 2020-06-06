#pragma once

#include "wrappers/glfw.hpp"
#include <glm/glm.hpp>

namespace scene
{
    class Camera final
    {
    public:
        glfw::DoubleCoord lastPos;
        glm::vec3 position;
        glm::vec2 angles;
        glm::mat4 projection;
        glm::mat4 infiniteProjection;

        Camera(glfw::Window& window, float zFar);
        void update(glfw::Window& window, double delta);
        glm::mat4 getViewMatrix() const;
    };
}