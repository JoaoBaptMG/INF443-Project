#pragma once

#include <FastNoise/FastNoise.h>
#include <cstddef>

namespace scene
{
    class TerrainFunction final
    {
        float width, height;
        FastNoise noise;

    public:
        TerrainFunction() = default;
        explicit TerrainFunction(float width, float height, int seed = 0) 
            : width(width), height(height), noise(seed) {}
        float operator()(float x, float y);
    };
}
