#include "Lighting.hpp"

#include "resources/Cache.hpp"
#include <glm/gtx/transform.hpp>

using namespace scene;

constexpr float Edge = 8;

Lighting::Lighting(float xmin, float ymin, float zmin, float xmax, float ymax, float zmax, float resolution,
    glm::vec3 lightDirection) : lightDirection(lightDirection)
{
    // Load the view
    auto view = glm::lookAtRH(glm::vec3(0, 0, 0), lightDirection, glm::vec3(0, 1, 0));

    // Get the min and max extents
    glm::vec4 points[8];
    points[0] = glm::vec4(xmin, ymin, zmin, 1);
    points[1] = glm::vec4(xmax, ymin, zmin, 1);
    points[2] = glm::vec4(xmin, ymax, zmin, 1);
    points[3] = glm::vec4(xmax, ymax, zmin, 1);
    points[4] = glm::vec4(xmin, ymin, zmax, 1);
    points[5] = glm::vec4(xmax, ymin, zmax, 1);
    points[6] = glm::vec4(xmin, ymax, zmax, 1);
    points[7] = glm::vec4(xmax, ymax, zmax, 1);

    glm::vec3 min = view * points[0], max = view * points[0];
    for (std::size_t i = 1; i < 8; i++)
    {
        auto vec = view * points[i];

        min.x = std::min(min.x, vec.x);
        min.y = std::min(min.y, vec.y);
        min.z = std::min(min.z, vec.z);

        max.x = std::max(max.x, vec.x);
        max.y = std::max(max.y, vec.y);
        max.z = std::max(max.z, vec.z);
    }

    // Generate the orthographic projection
    auto proj = glm::ortho(min.x - Edge, max.x + Edge, min.y - Edge, max.x + Edge, min.z - Edge, max.z + Edge);
    shadowMap.viewProjection = proj * view;

    // Create the depth texture
    shadowMap.width = std::ceil((max.x - min.x) / resolution);
    shadowMap.height = std::ceil((max.y - min.y) / resolution);
    shadowMap.depthTexture.assign(0, gl::InternalFormat::Depth32f, shadowMap.width, shadowMap.height);
    shadowMap.depthTexture.setMagFilter(gl::MagFilter::Linear);
    shadowMap.depthTexture.setMinFilter(gl::MinFilter::Linear);
    shadowMap.depthTexture.enableComparisonMode();
    shadowMap.depthTexture.setName("Shadow Depth Texture");

    // And attach it to the framebuffer
    shadowMap.framebuffer.attach(gl::Framebuffer::DepthAttachment, shadowMap.depthTexture);
    shadowMap.framebuffer.setName("Shadow Framebuffer");

    // Tell OpenGL not to draw anything to color
    shadowMap.framebuffer.bind();
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    gl::Framebuffer::bindDefault();
}

glm::mat4 Lighting::getShadowProjection() const
{
    return shadowMap.viewProjection;
}

void Lighting::setLightParams(gl::Program& program, const glm::mat4& view) const
{
    program.setUniform("Material.specularColor", glm::vec3(0.25, 0.25, 0.25));
    program.setUniform("Material.shininess", 4.5f);
    program.setUniform("Light.ambient", glm::vec3(0.25, 0.25, 0.25));
    program.setUniform("Light.diffuse", glm::vec3(0.625, 0.625, 0.625));
    program.setUniform("Light.specular", glm::vec3(1.0, 1.0, 1.0));
    program.setUniform("Light.directionView", glm::normalize(glm::mat3(view) * lightDirection));
    program.setUniform("ShadowViewProjection", shadowMap.viewProjection);

    shadowMap.depthTexture.bindTo(5);
    program.setUniform("ShadowMapTexture", 5);
}

void Lighting::beginShadow()
{
    shadowMap.framebuffer.bind();
    glViewport(0, 0, shadowMap.width, shadowMap.height);
    glClearDepth(1.0);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Lighting::endShadow()
{
    gl::Framebuffer::bindDefault();
}
