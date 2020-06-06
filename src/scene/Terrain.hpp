#pragma once

#include <vector>
#include <cmath>
#include "resources/Mesh.hpp"
#include "resources/Program.hpp"
#include "util/grid.hpp"
#include <glm/vec3.hpp>
#include <mutex>
#include <atomic>
#include <memory>
#include "TerrainFunction.hpp"
#include "Lighting.hpp"

namespace scene
{
    class Terrain final
    {
        std::shared_ptr<gl::Program> terrainProgram;
        std::shared_ptr<gl::Program> treesProgram;

        using ssize = std::make_signed_t<std::size_t>;
        TerrainFunction terrainFunction;

        // Drawable meshes for generation
        std::vector<gl::MeshBuilder> temporaryBuilders;
        std::vector<gl::Mesh> terrainMeshes;

        // Dirt texture
        gl::Texture3D dirtTexture;

        // Drawing of trees
        gl::Mesh trunkMesh, coneMesh;
        gl::InstanceSet trunkInstances, coneInstances;

        // Used for collisions
        ssize xofs, yofs;
        util::grid<float> heights, nys;
        util::grid<float> treeHeights;
        float globalMinHeight, globalMaxHeight;
        float resolution;

        // Used for grid generation (and optimizations later)
        util::grid<float> maxHeight;
        util::grid<float> minHeight;
        std::vector<glm::vec3> terrainMin;
        std::vector<glm::vec3> terrainMax;

        // For tree animation
        std::vector<glm::vec3> treePositions;
        std::vector<glm::mat4> trunkTransforms;
        std::vector<glm::mat4> coneTransforms;
        glm::vec2 shearingEllipse;
        float shearingRotation, time;

        void buildTerrain(ssize xmin, ssize ymin, ssize xmax, ssize ymax, int seed, bool wi = false, bool wj = false);
        void buildTrees(int seed);
        void generateDirtTexture(int seed);

    public:
        Terrain() = default;
        Terrain(float width, float height, float resolution, int seed);

        void update(double delta);

        void setColors(const glm::u8vec4& grassColor, const glm::u8vec4& sandColor, const glm::u8vec4& mountainColor);
        void setClipPlane(const glm::vec4& plane);
        void draw(const glm::mat4& projection, const glm::mat4& view, const Lighting& lighting);

        auto getGlobalMinHeight() const { return globalMinHeight; }
        auto getGlobalMaxHeight() const { return globalMaxHeight; }

        float operator()(float x, float z) const;
    };
}
