#include "SkyClouds.hpp"

#include "resources/Cache.hpp"
#include <random>
#include <FastNoise/FastNoise.h>
#include "util/grid.hpp"

using namespace scene;

constexpr float Pi = 3.14159265359f;
constexpr float CloudSpeed = 0.0025f;

static auto generateNoise(int seed)
{
    // Generate a Perlin noise texture
    std::mt19937 random(seed);
    std::uniform_real_distribution dist(-1.0f, 1.0f);

    constexpr int Width = 128, Height = 128;
    util::grid<float> image(Width, Height);
    std::generate(image.begin(), image.end(), [&] { return dist(random); });

    // Generate the texture
    gl::Texture2D texture;
    texture.assign(0, gl::InternalFormat::R16s, image.width(), Height, gl::Format::Red, image.data());
    texture.generateMipmap();
    texture.setMagFilter(gl::MagFilter::Linear);
    texture.setMinFilter(gl::MinFilter::LinearMipLinear);
    texture.setWrapEffectS(gl::WrapEffect::Repeat);
    texture.setWrapEffectT(gl::WrapEffect::Repeat);
    texture.setMaxAnisotropy(16);

    return texture;
}

SkyClouds::SkyClouds(float y, int seed) : skyY(y), time(0.0)
{
    // Load the program
    cloudProgram = cache::loadProgram({
        "resources/shaders/clouds.vert",
        "resources/shaders/clouds.frag"
    });

    cloudMesh = gl::Mesh::empty();
    
    std::mt19937 random(seed);
    std::uniform_real_distribution agen(0.0f, Pi);
    for (std::size_t i = 0; i < std::size(cloudTextures); i++)
    {
        cloudTextures[i] = generateNoise(random());
        float angle = agen(random);
        displacements[i] = CloudSpeed * glm::vec2(std::cos(angle), std::sin(angle));
    }  
}

void SkyClouds::update(double delta)
{
    time += delta;
}

void SkyClouds::draw(const glm::mat4& projection, const glm::mat4& view)
{
    // Regenerate the mesh
    regenerateMesh(projection * view);

    // And then prepare the program to draw
    cloudProgram->use();
    cloudProgram->setUniform("Projection", projection);
    cloudProgram->setUniform("View", view);
    cloudProgram->setUniform("TextureScale", 16384.0f);
    cloudProgram->setUniform("DistanceFalloff", 131072.0f);

    // Draw the mesh using alpha blending
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    for (std::size_t i = 0; i < std::size(cloudTextures); i++)
    {
        cloudTextures[i].bindTo(0);
        cloudProgram->setUniform("CloudTexture", 0);
        cloudProgram->setUniform("Displacement", time * displacements[i]);
        cloudMesh.draw(glm::mat4(1.0));
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_CLAMP);
}

static auto pointAtScreenCoord(glm::vec2 coord, float y0, const glm::mat4& viewProj)
{
    // Generate the matrix and the vector
    auto mat = glm::mat4(-viewProj[0], -viewProj[2], glm::vec4(coord.x, coord.y, 0, 1), glm::vec4(0, 0, 1, 0));
    auto inv = glm::inverse(mat);
    auto sol = inv * (viewProj[1] * y0 + viewProj[3]);

    return std::make_pair(glm::vec4(sol[0], y0, sol[1], 1), sol[3] / sol[2]);
}

static auto pointAtHorizonWithX(float x, const glm::mat4& viewProj)
{
    auto mat = glm::mat4(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), -viewProj[0], -viewProj[2]);
    auto inv = glm::inverse(mat);
    auto sol = inv * -glm::vec4(x, 0, 0, 1);
    return glm::vec4(sol[2], 0, sol[3], 0);
}

static auto pointAtHorizonWithY(float y, const glm::mat4& viewProj)
{
    auto mat = glm::mat4(glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 1, 0), -viewProj[0], -viewProj[2]);
    auto inv = glm::inverse(mat);
    auto sol = inv * -glm::vec4(0, y, 0, 1);
    return glm::vec4(sol[2], 0, sol[3], 0);
}

void SkyClouds::regenerateMesh(const glm::mat4& viewProj)
{
    // Get the four points (already in homogeneous coordinates)
    auto [ptbl, zbl] = pointAtScreenCoord(glm::vec2(-1, -1), skyY, viewProj);
    auto [ptbr, zbr] = pointAtScreenCoord(glm::vec2(1, -1), skyY, viewProj);
    auto [pttl, ztl] = pointAtScreenCoord(glm::vec2(-1, 1), skyY, viewProj);
    auto [pttr, ztr] = pointAtScreenCoord(glm::vec2(1, 1), skyY, viewProj);

    // This only works for OpenGL!
    auto infLeft = pointAtHorizonWithX(-1, viewProj);
    auto infRight = pointAtHorizonWithX(1, viewProj);
    auto infBottom = pointAtHorizonWithY(-1, viewProj);
    auto infTop = pointAtHorizonWithY(1, viewProj);

    gl::MeshBuilder mesh;

    // Now, we're going to generate each of the cases -- this is really some clipping
    std::size_t index = (std::abs(zbl) >= 1) + (std::abs(zbr) >= 1) * 2
        + (std::abs(ztl) >= 1) * 4 + (std::abs(ztr) >= 1) * 8;

    switch (index)
    {
    case 0: // All points are visible to the camera
        mesh.positionsH = { ptbl, ptbr, pttr, pttl }; break;
    case 1: // The bottom left point is not visible
        mesh.positionsH = { infLeft, infBottom, ptbr, pttr, pttl }; break;
    case 2: // The bottom right point is not visible
        mesh.positionsH = { ptbl, infBottom, infRight, pttr, pttl }; break;
    case 3: // The bottom two points are not visible
        mesh.positionsH = { infLeft, infRight, pttr, pttl }; break;
    case 4: // The top left point is not visible
        mesh.positionsH = { ptbl, ptbr, pttr, infTop, infLeft }; break;
    case 5: // The top and bottom left points are not visible
        mesh.positionsH = { infBottom, ptbr, pttr, infTop }; break;
    // case 6: the top left and bottom right points are not visible, this case is impossible
    case 7: // the top left, bottom left and bottom right points are not visible
        mesh.positionsH = { infTop, infRight, pttr }; break;
    case 8: // the top right point is not visible
        mesh.positionsH = { ptbl, ptbr, infRight, infTop, pttl }; break;
    // case 9: the top right and bottom left points are not visible, another impossible case
    case 10: // the top right and bottom right points are not visible
        mesh.positionsH = { ptbl, infBottom, infTop, pttl }; break;
    case 11: // the top right, bottom right and bottom left points are not visible
        mesh.positionsH = { infLeft, infTop, pttl }; break;
    case 12: // the top left and top right points are not visible
        mesh.positionsH = { ptbl, ptbr, infRight, infLeft }; break;
    case 13: // the top left, top right and bottom left points are not visible
        mesh.positionsH = { ptbl, infBottom, infLeft }; break;
    case 14: // the top left, top right and bottom right points are not visible
        mesh.positionsH = { infBottom, ptbr, infRight }; break;
    // case 15: none of the points are visible to the camera, just don't draw anything
    }

    // Swap the triangles if the matrix is "inverting" (det > 0 since the "normal" matrix flips the z coordinate)
    if (glm::determinant(viewProj) > 0)
        std::reverse(mesh.positionsH.begin(), mesh.positionsH.end());

    // And reassign the stream
    cloudMesh.streamMesh(mesh, gl::PrimitiveType::TriangleFan);
}