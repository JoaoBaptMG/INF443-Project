#include "Birds.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <random>
#include <numeric>

#include "resources/Cache.hpp"
#include "util/range.hpp"

constexpr float AnimationSpeed = 6;
constexpr int MinBirds = 8, MaxBirds = 24;
constexpr float Pi = 3.14159265359f;
constexpr float MinRadius = 64;
constexpr float PointPerturbation = 16;
constexpr float MaxHeight = 180;
constexpr float BirdSpeed = 18;

#ifdef _WIN32
#include <execution>
#define POLICY std::execution::par_unseq, 
#else
#define POLICY
#endif

using namespace scene;

Birds::Birds(const Terrain& terrain, int seed, float xmin, float zmin, float xmax, float zmax)
    : time(0)
{
    // Load the bird model
    birdModel = cache::load<model::Model>("resources/models/bird/scene.gltf");

    std::mt19937 random(seed);

    std::size_t numBirds = std::uniform_int_distribution(MinBirds, MaxBirds)(random);
    std::uniform_real_distribution xgen(xmin + 2 * MinRadius, xmax - 2 * MinRadius);
    std::uniform_real_distribution zgen(zmin + 2 * MinRadius, zmax - 2 * MinRadius);
    std::uniform_real_distribution hgen(terrain.getGlobalMaxHeight() + 16, MaxHeight);
    std::uniform_real_distribution agen(0.0f, 2 * Pi);
    std::uniform_real_distribution pgen(-PointPerturbation, PointPerturbation);

    birdPaths.resize(numBirds);
    birdTimes.resize(numBirds);
    birdPositions.resize(numBirds);
    birdVelocities.resize(numBirds);

    // Generate each path
    for (auto& path : birdPaths)
    {
        // Get the center
        auto center = glm::vec3(xgen(random), hgen(random), zgen(random));

        // Generate an ellipse
        float maxRadius = std::min({ center.x - xmin, xmax - center.x, center.z - zmin, zmax - center.z });
        std::uniform_real_distribution radiusGen(MinRadius, maxRadius);

        float r1 = radiusGen(random), r2 = radiusGen(random), angle = agen(random);
        float ca = std::cos(angle), sa = std::sin(angle);

        // Generate 16 control points for the cubic B�zier
        path.resize(25);
        for (std::size_t i = 0; i < 16; i++)
        {
            auto j = 3 * (i / 2) + i % 2 + 1;
            float t = 2 * Pi * j / 24;

            auto bp = glm::vec2(r1 * std::cos(t), r2 * std::sin(t));
            path[j] = center + glm::vec3(bp.x * ca + bp.y * sa, 0, -bp.x * sa + bp.y * ca);

            // Perturbate the paths a bit
            path[j] += glm::vec3(pgen(random), pgen(random), pgen(random));
        }

        // The initial points are the arithmetic mean of the control points
        for (std::size_t i = 0; i < 8; i++)
            path[3 * i] = (path[(3 * i + 23) % 24] + path[3 * i + 1]) / 2.0f;

        path[24] = path[0];
    }
}

void Birds::update(const Terrain& terrain, double delta)
{
    time += delta;
    birdModel->setTime(AnimationSpeed * time);

    util::range rng(std::size_t(0), birdTimes.size());
    std::for_each(POLICY rng.begin(), rng.end(), [&](std::size_t i)
        { 
            birdTimes[i] = nextStep(birdPaths[i], birdTimes[i], BirdSpeed * delta); 
            birdPositions[i] = birdPosition(birdPaths[i], birdTimes[i]);
            birdVelocities[i] = birdVelocity(birdPaths[i], birdTimes[i]);
        });
}

void Birds::setClipPlane(const glm::vec4& plane)
{
    birdModel->program->setUniform("ClipPlane", plane);
}

void Birds::draw(const glm::mat4& projection, const glm::mat4& view, const Lighting& lighting)
{
    birdModel->program->use();
    lighting.setLightParams(*birdModel->program, view);
    birdModel->program->setUniform("Projection", projection);
    birdModel->program->setUniform("View", view);

    for (std::size_t i = 0; i < birdPaths.size(); i++)
    {
        auto pos = birdPositions[i];
        auto vel = birdVelocities[i];
        birdModel->draw(glm::inverse(glm::lookAt(pos, pos - vel, glm::vec3(0, 1, 0))));
    }
    
}

glm::vec3 Birds::birdPosition(const std::vector<glm::vec3>& points, float t) const
{
    // Generate the position by picking the appropriate function
    auto numSegments = (points.size() - 1) / 3;
    t -= numSegments * std::size_t(std::floor(t / numSegments));
    auto segment = std::floor(t);

    auto a = points[3 * segment];
    auto b = points[3 * segment + 1];
    auto c = points[3 * segment + 2];
    auto d = points[3 * segment + 3];

    t -= segment;
    auto ct = 1 - t;

    // Cubic B�zier curve
    return a * ct * ct * ct + 3.0f * b * ct * ct * t + 3.0f * c * ct * t * t + d * t * t * t;
}

glm::vec3 Birds::birdVelocity(const std::vector<glm::vec3>& points, float t) const
{
    // Generate the position by picking the appropriate function
    auto numSegments = (points.size() - 1) / 3;
    t -= numSegments * std::size_t(std::floor(t / numSegments));
    auto segment = std::floor(t);

    auto a = points[3 * segment];
    auto b = points[3 * segment + 1];
    auto c = points[3 * segment + 2];
    auto d = points[3 * segment + 3];

    t -= segment;
    auto ct = 1 - t;

    // Derivative of cubic B�zier curve
    return 3.0f * ((b - a) * ct * ct + 2.0f * (c - b) * ct * t + (d - c) * t * t);
}

// Try to find the next step that will give the required speed
float Birds::nextStep(const std::vector<glm::vec3>& points, float t, float intendedDelta) const
{
    float d2 = intendedDelta * intendedDelta;
    auto cur = birdPosition(points, t);
    float dt = 1;

    while (true)
    {
        auto next = birdPosition(points, t + dt);
        if (glm::distance2(cur, next) < d2) break;
        dt *= 2;
    }

    float t0 = t, t1 = t + dt;
    for (std::size_t i = 0; i < 10; i++)
    {
        float tk = (t0 + t1) / 2;
        auto next = birdPosition(points, tk);
        if (glm::distance2(cur, next) < d2)
            t0 = tk;
        else t1 = tk;
    }

    return (t0 + t1) / 2;
}

