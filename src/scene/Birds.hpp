#pragma once

#include "model/Model.hpp"
#include "Terrain.hpp"
#include "Lighting.hpp"
#include <vector>
#include <array>
#include <glm/glm.hpp>

namespace scene
{
    class Birds final
    {
        // The time elapsed
        double time;

        // Modeling of the birds
        std::vector<std::vector<glm::vec3>> birdPaths;
        std::vector<float> birdTimes;

        // Optimization
        std::vector<glm::vec3> birdPositions;
        std::vector<glm::vec3> birdVelocities;

    public:
        Birds() = default;
        Birds(const Terrain& terrain, int seed, float xmin, float zmin, float xmax, float zmax);

        void update(const Terrain& terrain, double delta);

        void setClipPlane(const glm::vec4& plane);
        void draw(const glm::mat4& projection, const glm::mat4& view, const Lighting& lighting);

        glm::vec3 birdPosition(const std::vector<glm::vec3>& points, float t) const;
        glm::vec3 birdVelocity(const std::vector<glm::vec3>& points, float t) const;

        float nextStep(const std::vector<glm::vec3>& points, float t, float intendedDelta) const;

        std::shared_ptr<model::Model> birdModel;
    };
}