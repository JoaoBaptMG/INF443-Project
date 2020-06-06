#pragma once

#include "resources/Mesh.hpp"
#include "resources/Program.hpp"
#include "resources/Texture.hpp"

namespace scene
{
    class SkyClouds final
    {
        // The (variable) sky mesh put on the sky
        gl::Mesh cloudMesh;
        std::shared_ptr<gl::Program> cloudProgram;
        gl::Texture2D cloudTextures[3];
        glm::vec2 displacements[3];

        float skyY, time;

    public:
        SkyClouds() = default;
        SkyClouds(float y, int seed);

        void update(double delta);

        void regenerateMesh(const glm::mat4& viewProj);
        void draw(const glm::mat4& projection, const glm::mat4& view);
    };
}