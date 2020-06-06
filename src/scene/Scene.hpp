#pragma once

#include "wrappers/glfw.hpp"
#include "resources/Program.hpp"
#include "resources/FileUtils.hpp"
#include "resources/Mesh.hpp"

#include "Lighting.hpp"
#include "Terrain.hpp"
#include "Camera.hpp"
#include "SkyDome.hpp"
#include "SkyClouds.hpp"
#include "Water.hpp"
#include "Birds.hpp"

namespace scene
{
    class Scene final
    {
        glfw::Window& window;

        Lighting lighting;
        SkyDome skyDome;
        SkyClouds skyClouds;
        Terrain terrain;
        Camera camera;
        Water water;
        Birds birds;

        float time;

    public:
        Scene(glfw::Window& window);

        void update(double delta);
        void draw(double delta);
        void drawScene(const glm::mat4& projection, const glm::mat4& view, bool drawDome = true);
    };
}
