#pragma once

#include <memory>
#include "resources/Mesh.hpp"
#include "resources/Program.hpp"

namespace scene
{
    class SkyDome final
    {
        // The sphere mesh used to draw the dome
        gl::Mesh sphereMesh;

        // The horizontal scaling factor
        glm::vec2 scaling;

        // The program used
        std::shared_ptr<gl::Program> domeProgram;

    public:
        SkyDome() = default;
        SkyDome(std::size_t numDivs);
        
        // Set the drawing parameters
        void setProjection(const glm::mat4& proj)
        {
            domeProgram->use();
            domeProgram->setUniform("Projection", proj);

            // Adjust the scaling factor accordingly
            scaling.x = 1.0f / proj[0][0];
            scaling.y = 1.0f / proj[1][1];
        }

        void setColors(glm::u8vec4 horizonColor, glm::u8vec4 pinnacleColor)
        {
            domeProgram->setUniform("HorizonColor", glm::vec4(horizonColor) / 255.0f);
            domeProgram->setUniform("PinnacleColor", glm::vec4(pinnacleColor) / 255.0f);
        }

        void draw(const glm::mat4& projection, const glm::mat4& view);
    };
}
