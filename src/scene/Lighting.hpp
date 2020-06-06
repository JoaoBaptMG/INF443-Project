#pragma once

#include "resources/Program.hpp"
#include "resources/Texture.hpp"
#include "resources/Framebuffer.hpp"
#include <memory>

namespace scene
{
    class Lighting final
    {

        // Details for the shadow map
        struct ShadowMap
        {
            glm::mat4 viewProjection;
            gl::Framebuffer framebuffer;
            gl::Texture2D depthTexture;
            GLsizei width, height;
        } shadowMap;

        glm::vec3 lightDirection;

    public:
        Lighting() = default;
        Lighting(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax, float resolution, 
            glm::vec3 lightDirection);

        glm::mat4 getShadowProjection() const;
        void setLightParams(gl::Program& program, const glm::mat4& view) const;

        void beginShadow();
        void endShadow();
    };
}