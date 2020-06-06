#pragma once

#include "resources/Mesh.hpp"
#include <glm/glm.hpp>

namespace mesh_utils
{
    gl::MeshBuilder circle(glm::vec3 p0, glm::vec3 dir, float radius, glm::u8vec4 color,
        std::size_t numSegments = std::numeric_limits<std::size_t>::max());

    gl::MeshBuilder openCylinder(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color,
        std::size_t numSegments = std::numeric_limits<std::size_t>::max());

    gl::MeshBuilder closedCylinder(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color,
        std::size_t numSegments = std::numeric_limits<std::size_t>::max());

    gl::MeshBuilder openCone(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color,
        std::size_t numSegments = std::numeric_limits<std::size_t>::max());

    gl::MeshBuilder closedCone(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color,
        std::size_t numSegments = std::numeric_limits<std::size_t>::max());

    gl::MeshBuilder sphere(float radius, glm::u8vec4 color, std::size_t numSegments, std::size_t numSubdivisions);

    gl::MeshBuilder planeY(float y, float xmin, float zmin, float xmax, float zmax, glm::u8vec4 color);

    void swapWinding(gl::MeshBuilder& mesh);
}
