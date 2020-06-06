#include "SkyDome.hpp"

#include "mesh_utils.hpp"
#include "colors.hpp"
#include "resources/Cache.hpp"

#include <glm/gtx/transform.hpp>

using namespace scene;

constexpr float SphereRadius = 64.0f;

SkyDome::SkyDome(std::size_t numDivs)
{
    // Create the sphere
    // (numDivs / 2) | 1 will always be an odd number
    auto sphere = mesh_utils::sphere(SphereRadius, colors::White, numDivs, (numDivs / 2) | 1);
    sphere.colors.clear();
    mesh_utils::swapWinding(sphere);
    sphereMesh = gl::Mesh(sphere);
    sphereMesh.setName("Dome Mesh");

    // Create the program
    domeProgram = cache::loadProgram({
        "resources/shaders/position.vert",
        "resources/shaders/positionOnly.vert",
        "resources/shaders/dome.frag" });
    domeProgram->setName("Dome Program");
}

void SkyDome::draw(const glm::mat4& projection, const glm::mat4& view)
{
    glDepthMask(GL_FALSE);

    // Use the program
    domeProgram->use();
    domeProgram->setUniform("Projection", projection);
    domeProgram->setUniform("SphereRadius", SphereRadius);

    // Extract the translation and set the uniform to the matrix minus the translation
    auto trans = glm::vec3(view[3]);
    domeProgram->setUniform("View", glm::translate(-trans) * view);

    // Now, draw the mesh
    sphereMesh.draw(glm::mat4(1.0));

    glDepthMask(GL_TRUE);
}
