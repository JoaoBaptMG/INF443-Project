#include "Scene.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "resources/Framebuffer.hpp"
#include "resources/Query.hpp"
#include "resources/Cache.hpp"

#include "mesh_utils.hpp"
#include "colors.hpp"

#include "ImGui.hpp"

#include <random>
#include <iostream>
#include <queue>

std::queue<gl::Query> queries;

using namespace scene;

constexpr float TerrainWidth = 512;
constexpr float TerrainHeight = 512;

const glm::vec3 LightDirection = glm::normalize(glm::vec3(1, -1, -1));

Scene::Scene(glfw::Window& window) : window(window), time(0), camera(window, std::hypot(TerrainWidth, TerrainHeight))
{
    std::random_device random{};

    skyDome = SkyDome(32);
    skyDome.setColors(colors::LightBlue, colors::Blue);
    skyClouds = SkyClouds(500, random());
    terrain = Terrain(TerrainWidth, TerrainHeight, 0.5, random());
    water = Water(0, -TerrainWidth / 2, -TerrainHeight / 2, TerrainWidth / 2, TerrainHeight / 2, window.getFramebufferSize(), random());

    birds = Birds(terrain, random(), -TerrainWidth / 2, -TerrainHeight / 2, TerrainWidth / 2, TerrainHeight / 2);

    float minh = terrain.getGlobalMinHeight();
    lighting = Lighting(-TerrainWidth / 2, minh, -TerrainHeight / 2, TerrainWidth / 2, 200, TerrainHeight / 2, 1 / 8.0f, LightDirection);
    camera.position.y = terrain(camera.position.x, camera.position.z) + 16;

    terrain.setColors(colors::PastelGreen, colors::SandDune, colors::DarkBrown);
}

void Scene::update(double delta)
{
    time += delta;
    camera.update(window, delta);
    skyClouds.update(delta);
    terrain.update(delta);
    birds.update(terrain, delta);
    water.update(delta);
}

#include <imgui/imgui.h>

void Scene::draw(double delta)
{
    GLuint64 value = -1;

    while (!queries.empty() && queries.front().available())
    {
        value = queries.front().result();
        queries.pop();
    }

    if (value != (GLuint64)-1)
    {
        ImGui::Begin("Performance counter", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("GPU query timer: %d.%04d ms", value / 1000000, (value / 100) % 10000);
        ImGui::End();
    }

    queries.emplace(gl::QueryType::TimeElapsed);
    queries.back().begin();
    // Draw the shadow map
    lighting.beginShadow();
    drawScene(lighting.getShadowProjection(), glm::mat4(1.0), false);
    lighting.endShadow();
    window.setViewport();

    auto view = camera.getViewMatrix();

    gl::Framebuffer::bindDefault();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawScene(camera.projection, view);

    // Check for water occlusion
    water.checkOcclusion(camera.projection, view);

    if (water.shouldDraw())
    {
        // Draw the water reflection
        water.beginReflection();
        terrain.setClipPlane(water.getReflectionClipPlane());
        birds.setClipPlane(water.getReflectionClipPlane());
        drawScene(camera.projection, view * water.getReflectionMatrix());
        water.endReflection();

        // Draw the water refraction
        water.beginRefraction();
        terrain.setClipPlane(water.getRefractionClipPlane());
        birds.setClipPlane(water.getRefractionClipPlane());
        drawScene(camera.projection, view, false);
        water.endRefraction();

        gl::Framebuffer::bindDefault();
        water.draw(camera.projection, view);
    }
    queries.back().end();
}

void Scene::drawScene(const glm::mat4& projection, const glm::mat4& view, bool drawDome)
{
    if (drawDome)
    {
        skyDome.draw(camera.infiniteProjection, view);
        skyClouds.draw(camera.infiniteProjection, view);
    }

    // Set the terrain parameters
    terrain.draw(projection, view, lighting);
    birds.draw(projection, view, lighting);
}
