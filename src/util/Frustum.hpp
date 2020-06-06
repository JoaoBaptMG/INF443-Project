#pragma once

#include <glm/glm.hpp>
#include <algorithm>

namespace util
{
    // Check the minimum plane distance for an AABB
    float planeDistanceAABB(const glm::vec4& plane, const glm::vec3& min, const glm::vec3& max)
    {
        // Since the distance function is linear (thus continuous and monotonic), it attains
        // its minimum at the vertices of the AABB (which is convex), so we only need to test those
        return std::max({
            glm::dot(plane, glm::vec4(min.x, min.y, min.z, 1)),
            glm::dot(plane, glm::vec4(max.x, min.y, min.z, 1)),
            glm::dot(plane, glm::vec4(min.x, max.y, min.z, 1)),
            glm::dot(plane, glm::vec4(max.x, max.y, min.z, 1)),
            glm::dot(plane, glm::vec4(min.x, min.y, max.z, 1)),
            glm::dot(plane, glm::vec4(max.x, min.y, max.z, 1)),
            glm::dot(plane, glm::vec4(min.x, max.y, max.z, 1)),
            glm::dot(plane, glm::vec4(max.x, max.y, max.z, 1))
        });
    }

    struct Frustum final
    {
        glm::vec4 left, right, bottom, top, near, far;

        bool checkIntersectionAABB(const glm::vec3& min, const glm::vec3& max)
        {
            // Check for each plane if it has a positive distance
            for (const auto& plane : { left, right, bottom, top, near, far })
                if (planeDistanceAABB(plane, min, max) <= 0) return false;

            return true;
        }
    };

    // Returns the (vector) equations of the planes in homogeneous coordinates
    static inline Frustum frustumPlanes(const glm::mat4& viewProj)
    {
        auto pvt = glm::transpose(viewProj);

        return 
        {
            pvt[3] + pvt[0], pvt[3] - pvt[0],
            pvt[3] + pvt[1], pvt[3] - pvt[1],
            pvt[3] + pvt[2], pvt[3] - pvt[2]
        };
    }
}