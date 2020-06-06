#include "Water.hpp"

#include <glm/gtx/transform.hpp>
#include "mesh_utils.hpp"
#include "colors.hpp"
#include "resources/Cache.hpp"

#include "util/grid.hpp"
#include <FastNoise/FastNoise.h>
#include <random>

using namespace scene;

constexpr float Pi = 3.14159265359f;

Water::Water(float y, float xmin, float zmin, float xmax, float zmax, glfw::Size fbsize, int seed)
    : y(y), xmin(xmin), zmin(zmin), xmax(xmax), zmax(zmax), time(0), lastOcclusionValue(false)
{
    // Use a random value to provide water offsets
    {
        std::mt19937 random(seed);
        std::uniform_real_distribution val(0.0f, Pi);

        for (std::size_t i = 0; i < 2; i++)
        {
            float angle = val(random);
            waterVelocities[i] = (0.25f - 0.125f * i) * glm::vec2(std::cos(angle), std::sin(angle));
        }
    }

    // Create the attachments
    recreateAttachments(fbsize);

    // Attach them to the framebuffers
    for (auto fb : { &reflection, &refraction })
    {
        fb->color.setMagFilter(gl::MagFilter::Nearest);
        fb->color.setMinFilter(gl::MinFilter::Nearest);
        fb->color.setWrapEffectS(gl::WrapEffect::MirroredRepeat);
        fb->color.setWrapEffectT(gl::WrapEffect::MirroredRepeat);

        fb->framebuffer.attach(gl::Framebuffer::ColorAttachment(0), fb->color);
        fb->framebuffer.attach(gl::Framebuffer::DepthAttachment, fb->depth);
    }

    reflection.color.setName("Reflection Color Texture");
    reflection.depth.setName("Reflection Depth Renderbuffer");
    reflection.framebuffer.setName("Reflection Framebuffer");
    reflection.color.setName("Refraction Color Texture");
    reflection.depth.setName("Refraction Depth Renderbuffer");
    reflection.framebuffer.setName("Refraction Framebuffer");

    // Create the mesh
    auto mesh = mesh_utils::planeY(y, xmin, zmin, xmax, zmax, colors::LightBlue);
    mesh.colors.clear();
    mesh.normals.clear();
    waterMesh = mesh;
    waterMesh.setName("Water Mesh");

    // Create the program
    waterProgram = cache::loadProgram({
        "resources/shaders/lighting.frag",
        "resources/shaders/water.vert",
        "resources/shaders/water.frag" });
    waterProgram->setName("Water Program");

    queryProgram = cache::loadProgram({
        "resources/shaders/position.vert",
        "resources/shaders/positionOnly.vert",
        "resources/shaders/noop.frag" });

    generateRipple(seed);
}

void Water::generateRipple(int seed)
{
    for (std::size_t k = 0; k < 2; k++)
    {
        // Generate the ripple texture
        util::grid<float> heightmaps(256, 256);
        FastNoise noise(seed);
        for (std::size_t j = 0; j < 256; j++)
            for (std::size_t i = 0; i < 256; i++)
                heightmaps(i, j) = 0.8f * noise.GetPerlin(8 * i, 8 * j, 14 * k)
                    + 0.4f * noise.GetPerlin(16 * i, 16 * j, 28 * k);

        util::grid<glm::vec2> normals(256, 256);
        for (std::size_t j = 0; j < 256; j++)
            for (std::size_t i = 0; i < 256; i++)
            {
                glm::vec3 gx, gy;

                if (i == 0) gx = glm::vec3(1, 0, heightmaps(i + 1, j) - heightmaps(255, j));
                else if (i == 255) gx = glm::vec3(1, 0, heightmaps(0, j) - heightmaps(i - 1, j));
                else gx = glm::vec3(1, 0, heightmaps(i + 1, j) - heightmaps(i - 1, j));

                if (j == 0) gy = glm::vec3(0, 1, heightmaps(i, j + 1) - heightmaps(i, 255));
                else if (j == 255) gy = glm::vec3(0, 1, heightmaps(i, 0) - heightmaps(i, j - 1));
                else gy = glm::vec3(0, 1, heightmaps(i, j + 1) - heightmaps(i, j - 1));

                auto normal = glm::normalize(glm::cross(gx, gy));
                normals(i, j) = glm::vec2(normal.x, normal.y);
            }

        rippleTextures[k].assign(0, gl::InternalFormat::RG16s, 256, 256, gl::Format::RG, &normals(0, 0).x);
        rippleTextures[k].generateMipmap();
        rippleTextures[k].setMagFilter(gl::MagFilter::Linear);
        rippleTextures[k].setMinFilter(gl::MinFilter::LinearMipLinear);
        rippleTextures[k].setMaxAnisotropy(16);
        rippleTextures[k].setWrapEffectS(gl::WrapEffect::Repeat);
        rippleTextures[k].setWrapEffectT(gl::WrapEffect::Repeat);
        rippleTextures[k].setName("Ripple texture #" + std::to_string(k));
    }
}

void Water::recreateAttachments(glfw::Size fbsize)
{
    for (auto fb : { &reflection, &refraction })
    {
        fb->color.assign(0, gl::InternalFormat::RGBA8, fbsize.width, fbsize.height);
        fb->depth.storage(gl::InternalFormat::Depth, fbsize.width, fbsize.height);
    }
}

glm::mat4 Water::getReflectionMatrix() const
{
    // Translate so y goes to 0, scale negatively on y and translate back
    // (remember the transformations are read right to left)
    return glm::translate(glm::vec3(0, y, 0)) * glm::scale(glm::vec3(1, -1, 1)) * glm::translate(glm::vec3(0, -y, 0));
}

// This is so the reflection texture "bleeds" a bit to mask the issues that arise with normal bumping
glm::vec4 Water::getReflectionClipPlane() const { return glm::vec4(0, 1, 0, -y + 3); }

void Water::beginReflection()
{
    // Enable the first clip plane
    glEnable(GL_CLIP_DISTANCE0);

    // Change the front face winding
    glFrontFace(GL_CW);

    // Bind the framebuffer
    reflection.framebuffer.bind();

    // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::endReflection()
{
    // Reverse the operations
    gl::Framebuffer::bindDefault();
    glFrontFace(GL_CCW);
    glDisable(GL_CLIP_DISTANCE0);
}

// This is so the refraction texture "bleeds" a bit to mask the issues that arise with normal bumping
glm::vec4 Water::getRefractionClipPlane() const { return glm::vec4(0, -1, 0, y + 3); }

void Water::beginRefraction()
{
    // Enable the first clip plane
    glEnable(GL_CLIP_DISTANCE0);

    // Bind the framebuffer
    refraction.framebuffer.bind();

    // Clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::endRefraction()
{
    // Reverse the operations
    gl::Framebuffer::bindDefault();
    glDisable(GL_CLIP_DISTANCE0);
}

void Water::draw(const glm::mat4& projection, const glm::mat4& view)
{
    waterProgram->use();
    waterProgram->setUniform("Projection", projection);
    waterProgram->setUniform("View", view);
    waterProgram->setUniform("RepeatPeriod", 8.0f);
    waterProgram->setUniform("WaveHeight", 0.1f);
    waterProgram->setUniform("ViewNormal", glm::normalize(glm::vec3(view[1])));
    waterProgram->setUniform("Offsets[0]", time * waterVelocities[0]);
    waterProgram->setUniform("Offsets[1]", time * waterVelocities[1]);

    reflection.color.bindTo(0);
    waterProgram->setUniform("ReflectionTexture", 0);

    refraction.color.bindTo(1);
    waterProgram->setUniform("RefractionTexture", 1);

    rippleTextures[0].bindTo(2);
    waterProgram->setUniform("RippleTextures[0]", 2);

    rippleTextures[1].bindTo(3);
    waterProgram->setUniform("RippleTextures[1]", 3);

    // Draw the water at identity position
    waterMesh.draw(glm::mat4(1.0));
}

void Water::checkOcclusion(const glm::mat4& projection, const glm::mat4& view)
{
    // Create a new occlusion query
    gl::Query query(gl::QueryType::AnySamplesPassed);

    // Enable the query program
    queryProgram->use();
    queryProgram->setUniform("Projection", projection);
    queryProgram->setUniform("View", view);

    // Disable rendering
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    // Draw the water
    query.begin();
    waterMesh.draw(glm::mat4(1.0));
    query.end();

    // Reenable rendering
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

    // Push the query to the queue
    queries.push(std::move(query));
}

bool Water::shouldDraw()
{
    // Pass through all the queries
    while (!queries.empty() && queries.front().available())
    {
        lastOcclusionValue = queries.front().result();
        queries.pop();
    }

    return lastOcclusionValue;
}
