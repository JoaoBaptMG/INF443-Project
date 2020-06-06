#include "TerrainFunction.hpp"

using namespace scene;

#include <random>
#include <cmath>

constexpr float Pi = 3.14159265359f;
constexpr float KernelSize = 0.25f;
constexpr float BorderSize = 8;

static float flattenFunction(float x)
{
    if (x < 1 - KernelSize) return 0;
    return 0.5f + 0.5f * std::sin(Pi * (x - (1 - KernelSize / 2)) / KernelSize);
}

static float flattener(float x, float levelHeights, float maxHeight = std::numeric_limits<float>::infinity())
{
    if (x > maxHeight) return x;

    float v = x / levelHeights;
    float nv = std::floor(v) + flattenFunction(v - std::floor(v));
    return nv * levelHeights;
}

float lerp(float a, float b, float m)
{
    return a + m * (b - a);
}

float TerrainFunction::operator()(float x, float y)
{
    // Create the base heights
    float baseHeight = 160 * noise.GetPerlinFractal(x * 0.7f, y * 0.7f, 0) + 20;
    baseHeight = std::max(baseHeight, -20.0f);

    // Calculate the flattened height
    baseHeight = flattener(baseHeight, 20.0f, 140.0f);

    // Now, perturb the heights with a simplex noise
    float perturbation = 1 * noise.GetSimplex(x * 8, y * 8, 2.5f);

    // This should be the final height, but
    float th = baseHeight + perturbation + 4;
    return th;
}
