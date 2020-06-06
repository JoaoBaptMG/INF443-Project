#include "mesh_utils.hpp"

#include <cmath>

using namespace mesh_utils;

constexpr float Pi = 3.14159265359f;

static auto perpendicularBasis(glm::vec3 dir)
{
    // Compute two vectors that are perpendicular to dir and each other, used to make circles and ellipses
    // Those vectors are such that a x b is a vector parallel and in the same direction to dir
    auto perp = glm::cross(glm::vec3(1, 0, 0), dir);
    if (perp == glm::vec3()) perp = glm::cross(glm::vec3(0, 1, 0), dir);
    return std::make_pair(glm::normalize(perp), glm::normalize(glm::cross(perp, dir)));
}

gl::MeshBuilder mesh_utils::circle(glm::vec3 p0, glm::vec3 normal, float radius, glm::u8vec4 color, std::size_t numSegments)
{
    // Parameters
    if (normal == glm::vec3() || radius == 0.0) return gl::MeshBuilder();

    // Set the number of segments if default
    if (numSegments == std::numeric_limits<std::size_t>::max())
        numSegments = std::size_t(7.2 * radius);

    // Get a random perpendicular direction
    auto [px, py] = perpendicularBasis(normal);

    // Make the circle
    gl::MeshBuilder mesh;
    mesh.colors.resize(numSegments, color);
    mesh.normals.resize(numSegments, normal);
    mesh.positions.reserve(numSegments);
    mesh.indices.reserve(3 * (numSegments - 2));

    normal = glm::normalize(normal);

    for (std::size_t i = 0; i < numSegments; i++)
    {
        auto theta = 2 * Pi * i / numSegments;
        auto direction = px * std::cos(theta) + py * std::sin(theta);

        mesh.positions.push_back(p0 + direction * radius);
    }

    for (unsigned int i = 2; i < numSegments; i++)
    {
        mesh.indices.push_back(i - 1);
        mesh.indices.push_back(0);
        mesh.indices.push_back(i);
    }

    return mesh;
}

gl::MeshBuilder mesh_utils::openCylinder(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color, std::size_t numSegments)
{
    // Parameters
    if (p0 == p1 || radius == 0.0) return gl::MeshBuilder();

    // Set the number of segments if default
    if (numSegments == std::numeric_limits<std::size_t>::max())
        numSegments = std::size_t(7.2 * radius);

    // Get a random perpendicular direction
    auto [px, py] = perpendicularBasis(p1 - p0);

    // Make the cylinder
    gl::MeshBuilder mesh;
    mesh.colors.resize(2 * numSegments, color);
    mesh.positions.reserve(2 * numSegments);
    mesh.normals.reserve(2 * numSegments);
    mesh.indices.reserve(6 * numSegments);

    // Build the vertices and the topology
    for (std::size_t i = 0; i < numSegments; i++)
    {
        auto theta = 2 * Pi * i / numSegments;
        auto normal = px * std::cos(theta) + py * std::sin(theta);

        mesh.positions.push_back(p0 + normal * radius);
        mesh.positions.push_back(p1 + normal * radius);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);

        auto in = (i + 1) % numSegments;
        mesh.indices.push_back(2 * i);
        mesh.indices.push_back(2 * i + 1);
        mesh.indices.push_back(2 * in);
        mesh.indices.push_back(2 * i + 1);
        mesh.indices.push_back(2 * in + 1);
        mesh.indices.push_back(2 * in);
    }

    return mesh;
}

gl::MeshBuilder mesh_utils::closedCylinder(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color, std::size_t numSegments)
{
    return openCylinder(p0, p1, radius, color, numSegments)
        + circle(p0, p0 - p1, radius, color, numSegments)
        + circle(p1, p1 - p0, radius, color, numSegments);
}

gl::MeshBuilder mesh_utils::openCone(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color, std::size_t numSegments)
{
    // Parameters
    if (p0 == p1 || radius == 0.0) return gl::MeshBuilder();

    // Set the number of segments if default
    if (numSegments == std::numeric_limits<std::size_t>::max())
        numSegments = std::size_t(7.2 * radius);

    // Get a random perpendicular direction
    auto [px, py] = perpendicularBasis(p1 - p0);

    // Make the cylinder
    gl::MeshBuilder mesh;
    mesh.colors.resize(2 * numSegments, color);
    mesh.positions.reserve(2 * numSegments);
    mesh.normals.reserve(2 * numSegments);
    mesh.indices.reserve(3 * numSegments);

    // Build the vertices and the topology
    for (std::size_t i = 0; i < numSegments; i++)
    {
        auto theta = 2 * Pi * i / numSegments;
        auto dir = px * std::cos(theta) + py * std::sin(theta);
        auto dp = -px * std::sin(theta) + py * std::cos(theta);
        auto pos = p0 + dir * radius;
        auto normal = glm::normalize(glm::cross(p1 - pos, dp));

        mesh.positions.push_back(pos);
        mesh.positions.push_back(p1);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);

        auto in = (i + 1) % numSegments;
        mesh.indices.push_back(2 * i);
        mesh.indices.push_back(2 * i + 1);
        mesh.indices.push_back(2 * in);
    }

    return mesh;
}

gl::MeshBuilder mesh_utils::closedCone(glm::vec3 p0, glm::vec3 p1, float radius, glm::u8vec4 color, std::size_t numSegments)
{
    return openCone(p0, p1, radius, color, numSegments) + circle(p0, p0 - p1, radius, color, numSegments);
}

gl::MeshBuilder mesh_utils::sphere(float radius, glm::u8vec4 color, std::size_t numSegments, std::size_t numSubdivisions)
{
    gl::MeshBuilder mesh;

    // Put everything on its right size
    auto numVertices = numSegments * (numSubdivisions - 2) + 2;
    mesh.colors.resize(numVertices, color);
    mesh.positions.reserve(numVertices);
    mesh.normals.reserve(numVertices);

    // First, generate the two polar points
    mesh.positions.emplace_back(0, radius, 0);
    mesh.positions.emplace_back(0, -radius, 0);
    mesh.normals.emplace_back(0, 1, 0);
    mesh.normals.emplace_back(0, -1, 0);

    // Now, proceed to generate the remaining of the points
    for (std::size_t j = 1; j < numSubdivisions - 1; j++)
        for (std::size_t i = 0; i < numSegments; i++)
        {
            // Generate the vectors
            float theta = 2 * Pi * i / numSegments;
            float phi = Pi * j / numSubdivisions;

            float ct = std::cos(theta), st = std::sin(theta);
            float cp = std::cos(phi), sp = std::sin(phi);

            auto vec = glm::vec3(sp * ct, cp, sp * st);
            mesh.positions.push_back(radius * vec);
            mesh.normals.push_back(vec);
        }

    // Generate the mesh topology
    mesh.indices.reserve(3 * numSegments * (2 + 2 * (numSubdivisions - 3)));
    // Generate the polar segments
    for (std::size_t i = 0; i < numSegments; i++)
    {
        auto in = (i + 1) % numSegments;
        mesh.indices.push_back(0);
        mesh.indices.push_back(2 + in);
        mesh.indices.push_back(2 + i);
        mesh.indices.push_back(1);
        mesh.indices.push_back(2 + i + numSegments * (numSubdivisions - 3));
        mesh.indices.push_back(2 + in + numSegments * (numSubdivisions - 3));
    }

    // Now, generate the longitudinal segments
    for (std::size_t j = 0; j < numSubdivisions - 3; j++)
        for (std::size_t i = 0; i < numSegments; i++)
        {
            auto in = (i + 1) % numSegments;
            mesh.indices.push_back(2 + numSegments * j + i);
            mesh.indices.push_back(2 + numSegments * j + in);
            mesh.indices.push_back(2 + numSegments * (j + 1) + in);
            mesh.indices.push_back(2 + numSegments * j + i);
            mesh.indices.push_back(2 + numSegments * (j + 1) + in);
            mesh.indices.push_back(2 + numSegments * (j + 1) + i);
        }

    return mesh;
}

gl::MeshBuilder mesh_utils::planeY(float y, float xmin, float zmin, float xmax, float zmax, glm::u8vec4 color)
{
    gl::MeshBuilder mesh;

    // Generate the 4 vertices
    mesh.colors.resize(4, color);
    mesh.normals.resize(4, glm::vec3(0, 1, 0));

    mesh.positions.resize(4);
    mesh.positions[0] = glm::vec3(xmin, y, zmin);
    mesh.positions[1] = glm::vec3(xmax, y, zmin);
    mesh.positions[2] = glm::vec3(xmax, y, zmax);
    mesh.positions[3] = glm::vec3(xmin, y, zmax);

    // Topology
    mesh.indices = { 0, 2, 1, 0, 3, 2 };

    return mesh;
}

void mesh_utils::swapWinding(gl::MeshBuilder& mesh)
{
    auto numTris = mesh.indices.size() / 3;
    for (std::size_t i = 0; i < numTris; i++)
        std::swap(mesh.indices[3 * i + 1], mesh.indices[3 * i + 2]);
}
