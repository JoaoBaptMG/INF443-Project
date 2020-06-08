#include "Terrain.hpp"

#include "colors.hpp"

#include <thread>
#include <random>
#include <algorithm>
#include "resources/Cache.hpp"
#include "util/Frustum.hpp"
#include "util/range.hpp"
#include "mesh_utils.hpp"

#include <glm/gtx/transform.hpp>

#ifdef _WIN32
#include <execution>
#define POLICY std::execution::par_unseq, 
#else
#define POLICY
#endif

using namespace scene;

// This is so I can easily parallelize if necessary
static std::mutex terrainMutex;

constexpr std::make_signed_t<std::size_t> MaxCellDivision = 128;
constexpr float Pi = 3.14159265359f;

Terrain::Terrain(float width, float height, float resolution, int seed) : terrainFunction(width, height, seed),
    resolution(resolution), time(0)
{
    // Create the programs
    terrainProgram = cache::loadProgram({
        "resources/shaders/position.vert",
        "resources/shaders/lighting.frag",
        "resources/shaders/terrain.vert",
        "resources/shaders/terrain.frag" });
    terrainProgram->setName("Terrain Program");

    treesProgram = cache::loadProgram({
        "resources/shaders/position.vert",
        "resources/shaders/lighting.frag",
        "resources/shaders/commonObjects.vert",
        "resources/shaders/commonObjects.frag" });
    treesProgram->setName("Common Objects Program");

    auto hw = ssize(0.5f * width / resolution);
    auto hh = ssize(0.5f * height / resolution);

    heights.resize(2 * hw + 1, 2 * hh + 1);
    nys.resize(2 * hw + 1, 2 * hh + 1);
    xofs = hw, yofs = hh;

    // Generate meshes spanning at most 128*128 triangles
    auto divsX = (2 * hw + 1) / MaxCellDivision;
    auto divsY = (2 * hh + 1) / MaxCellDivision;

    maxHeight.resize(divsX, divsY);
    minHeight.resize(divsX, divsY);

    std::mt19937 engine(seed);

    // Simple paralellization
    std::vector<std::thread> threads;
    for (ssize j = 0; j < divsY; j++)
        for (ssize i = 0; i < divsX; i++)
        {
            if (i == 0 && j == 0) continue;

            auto x1 = -hw + MaxCellDivision * i;
            auto y1 = -hh + MaxCellDivision * j;
            auto x2 = std::min(hw, -hw + MaxCellDivision * (i + 1));
            auto y2 = std::min(hh, -hh + MaxCellDivision * (j + 1));
            int seed = engine();

            threads.push_back(std::thread([=, wi = i == divsX - 1, wj = j == divsY - 1]
                { buildTerrain(x1, y1, x2, y2, seed, wi, wj); }));
        }

    buildTerrain(-hw, -hh, std::min(hw, -hw + MaxCellDivision), std::min(hh, -hh + MaxCellDivision), engine());
    for (auto& thread : threads) thread.join();

    for (auto& builder : temporaryBuilders)
    {
        terrainMeshes.push_back(builder);
        terrainMeshes.back().setName("Terrain Mesh " + std::to_string(terrainMeshes.size()));
    }

    temporaryBuilders.clear();

    // Now, generate the mesh and the instances
    buildTrees(engine());

    // Compute the global min and max height
    globalMinHeight = *std::min_element(minHeight.begin(), minHeight.end());
    globalMaxHeight = *std::max_element(maxHeight.begin(), maxHeight.end());

    // Generate the dirt texture
    generateDirtTexture(engine());
}

void Terrain::buildTerrain(ssize xmin, ssize ymin, ssize xmax, ssize ymax, int seed, bool wi, bool wj)
{
    // Some guarantees
    if (xmin > xmax) std::swap(xmin, xmax);
    if (ymin > ymax) std::swap(ymin, ymax);
    if (xmin == xmax || ymin == ymax) return;

    auto width = xmax - xmin + 1;
    auto height = ymax - ymin + 1;

    // Threaded compute of our own min height
    float localMinHeight = std::numeric_limits<float>::infinity();
    float localMaxHeight = -std::numeric_limits<float>::infinity();

    // In this phase, I'll only have a single mesh
    gl::MeshBuilder mesh;

    // First, we're going to build the vertices
    mesh.positions.resize(width * height);
    for (auto j = ymin; j <= ymax; j++)
        for (auto i = xmin; i <= xmax; i++)
        {
            auto idx = (j - ymin) * width + (i - xmin);

            float x = i * resolution;
            float y = j * resolution;
            float height = terrainFunction(x, y);

            // Push the position
            mesh.positions[idx] = glm::vec3(x, height, -y);

            localMinHeight = std::min(localMinHeight, height);
            localMaxHeight = std::max(localMaxHeight, height);

            // Write to the grid - no data races here, each rectangle only writes its own part
            if ((wi || i < xmax) && (wj || j < ymax)) 
                heights(xofs + i, yofs + j) = height;
        }

    // The normals
    mesh.normals.resize(width * height);
    for (auto j = ymin; j <= ymax; j++)
        for (auto i = xmin; i <= xmax; i++)
        {
            auto idx = (j - ymin) * width + (i - xmin);

            // Do the grad calculation
            glm::vec3 gx, gy;

            // Sample outside of the map to have perfectly seamless normals
            if (i == xmin)
            {
                float x = (i - 1) * resolution;
                float y = j * resolution;
                float height = terrainFunction(x, y);
                gx = mesh.positions[idx + 1] - glm::vec3(x, height, -y);
            }
            else if (i == xmax)
            {
                float x = (i + 1) * resolution;
                float y = j * resolution;
                float height = terrainFunction(x, y);
                gx = glm::vec3(x, height, -y) - mesh.positions[idx - 1];
            }
            else gx = mesh.positions[idx + 1] - mesh.positions[idx - 1];

            if (j == ymin)
            {
                float x = i * resolution;
                float y = (j - 1) * resolution;
                float height = terrainFunction(x, y);
                gy = mesh.positions[idx + width] - glm::vec3(x, height, -y);
            }
            else if (j == ymax)
            {
                float x = i * resolution;
                float y = (j + 1) * resolution;
                float height = terrainFunction(x, y);
                gy = glm::vec3(x, height, -y) - mesh.positions[idx - width];
            }
            else gy = mesh.positions[idx + width] - mesh.positions[idx - width];

            mesh.normals[idx] = glm::normalize(glm::cross(gx, gy));

            // Write to the grid - no data races here, each rectangle only writes its own part
            if ((wi || i < xmax) && (wj || j < ymax)) 
                nys(xofs + i, yofs + j) = mesh.normals[idx].y;
        }

    // Now for the topology
    mesh.indices.reserve(6 * (width - 1) * (height - 1));
    for (ssize j = 1; j < height; j++)
        for (ssize i = 1; i < width; i++)
        {
            // Push the two triangles
            mesh.indices.push_back((j - 1) * width + (i - 1));
            mesh.indices.push_back((j - 1) * width + i);
            mesh.indices.push_back(j * width + i);
            mesh.indices.push_back((j - 1) * width + (i - 1));
            mesh.indices.push_back(j * width + i);
            mesh.indices.push_back(j * width + (i - 1));
        }

    // Compute the min and max height per grid
    ssize ci = (xofs + xmin) / MaxCellDivision, cj = (yofs + ymin) / MaxCellDivision;
    minHeight(ci, cj) = localMinHeight;
    maxHeight(ci, cj) = localMaxHeight;

    {
        std::lock_guard lock(terrainMutex);
        temporaryBuilders.emplace_back(std::move(mesh));

        // This will form the AABB for frustum culling
        terrainMin.emplace_back(xmin * resolution, localMinHeight, -ymax * resolution);
        terrainMax.emplace_back(xmax * resolution, localMaxHeight, -ymin * resolution);
    }
}

constexpr float TreeRadius = 3;
constexpr float MinTrunkHeight = 6, MaxTrunkHeight = 15;
constexpr float MinConeSize = 2, MaxConeSize = 7;

void Terrain::buildTrees(int seed)
{
    // Create the tree mesh
    trunkMesh = mesh_utils::openCylinder(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 1, colors::CocoaBrown, 32);
    coneMesh = mesh_utils::closedCone(glm::vec3(0, 0, 0), glm::vec3(0, 8, 0), 1, colors::DarkGreen, 32);
    trunkMesh.setName("Trunk Mesh");
    coneMesh.setName("Cone Mesh");

    // Generate a random number of trees
    std::mt19937 random(seed);

    auto width = heights.width(), height = heights.height();

    // Try to fill 0.05% of the terrain with trees
    ssize radius = std::ceil(TreeRadius / resolution);
    std::size_t numTrees = width * height / 2000;
    std::vector<glm::tvec2<ssize>> indices;
    indices.reserve((width - 2 * radius) * (height - 2 * radius));
    for (std::size_t j = radius; j < height - radius; j++)
        for (std::size_t i = radius; i < height - radius; i++)
            indices.emplace_back(i, j);
    std::shuffle(indices.begin(), indices.end(), random);

    std::uniform_real_distribution heightGen(MinTrunkHeight, MaxTrunkHeight);
    std::uniform_real_distribution sizeGen(MinConeSize, MaxConeSize);
    treeHeights.resize(width, height, 0);

    trunkTransforms.reserve(numTrees);
    coneTransforms.reserve(numTrees);
    for (const auto& vec : indices)
    {
        auto i = vec.x, j = vec.y;
        float h = std::min({ heights(i, j), heights(i + 1, j), heights(i - 1, j), heights(i, j - 1), heights(i, j + 1) });

        // Don't generate trees in water
        if (h < 0) continue;

        // Don't place trees in too step areas
        float ny = std::min({ nys(i, j), nys(i + 1, j), nys(i - 1, j), nys(i, j - 1), nys(i, j + 1) });
        if (ny < 0.75) continue;

        bool skipTree = false;

        // Don't place trees too close to each other
        for (ssize ci = -radius; ci <= radius; ci++)
        {
            ssize mj = radius - std::abs(ci);
            for (ssize cj = -mj; cj <= mj; cj++)
                if (treeHeights(i + ci, j + cj) > 0)
                {
                    skipTree = true;
                    break;
                }

            if (skipTree) break;
        }

        if (skipTree) continue;

        float trunkHeight = heightGen(random);
        float coneSize = sizeGen(random);
        treeHeights(i, j) = trunkHeight + 8;

        auto ci = i / MaxCellDivision, cj = j / MaxCellDivision;
        minHeight(ci, cj) = std::min(minHeight(ci, cj), h);
        maxHeight(ci, cj) = std::max(maxHeight(ci, cj), h + trunkHeight + 8);

        auto trunkPos = glm::vec3((i - xofs) * resolution, h, -(j - yofs) * resolution);
        treePositions.emplace_back(trunkPos);

        auto trunkScale = glm::vec3(1, trunkHeight, 1);
        trunkTransforms.emplace_back(glm::scale(trunkScale));

        auto conePos = glm::vec3(0, trunkHeight, 0);
        auto coneScale = glm::vec3(coneSize, 1, coneSize);
        coneTransforms.emplace_back(glm::translate(conePos) * glm::scale(coneScale));

        if (trunkTransforms.size() == numTrees) break;
    }

    // Generate the shearing parameters
    constexpr float MaxShearingRadius = 0.25;
    std::uniform_real_distribution radiusGen(0.0f, MaxShearingRadius);
    shearingEllipse = glm::vec2(radiusGen(random), radiusGen(random));
    shearingRotation = std::uniform_real_distribution(0.0f, 2 * Pi)(random);
}

constexpr GLsizei TextureSize = 64;
void Terrain::generateDirtTexture(int seed)
{
    std::mt19937 random(seed);
    std::uniform_real_distribution floatGen(0.4f, 1.0f);

    // Create the size texture
    std::vector<float> image(TextureSize * TextureSize * TextureSize);
    std::generate(image.begin(), image.end(), [&] { return floatGen(random); });

    // Configure the texture
    dirtTexture.assign(0, gl::InternalFormat::R16, TextureSize, TextureSize, TextureSize, gl::Format::Red, image.data());
    dirtTexture.generateMipmap();
    dirtTexture.setMagFilter(gl::MagFilter::Linear);
    dirtTexture.setMinFilter(gl::MinFilter::LinearMipLinear);
    dirtTexture.setMaxAnisotropy(16);
    dirtTexture.setWrapEffectS(gl::WrapEffect::Repeat);
    dirtTexture.setWrapEffectT(gl::WrapEffect::Repeat);
    dirtTexture.setWrapEffectR(gl::WrapEffect::Repeat);
}

void Terrain::update(double delta)
{
    time += delta;

    // Generate the final transform
    std::vector<glm::mat4> trunkFinalTransforms(treePositions.size());
    std::vector<glm::mat4> coneFinalTransforms(treePositions.size());

    auto angle = Pi * time / 2;
    auto shear = glm::vec2(std::cos(angle), std::sin(angle)) * shearingEllipse;
    auto cr = std::cos(shearingRotation), sr = std::sin(shearingRotation);
    auto shearing = glm::mat4(1.0);
    shearing[1][0] = cr * shear.x + sr * shear.y;
    shearing[1][2] = -sr * shear.x + cr * shear.y;

    util::range rng(std::size_t(0), treePositions.size());
    std::for_each(POLICY rng.begin(), rng.end(), [&](std::size_t i)
        {
            // Compute the final transforms
            trunkFinalTransforms[i] = glm::translate(treePositions[i]) * shearing * trunkTransforms[i];
            coneFinalTransforms[i] = glm::translate(treePositions[i]) * shearing * coneTransforms[i];
        });

    // Set the instances
    trunkInstances.setInstances(trunkFinalTransforms);
    coneInstances.setInstances(coneFinalTransforms);
}

void Terrain::setColors(const glm::u8vec4& grassColor, const glm::u8vec4& sandColor, const glm::u8vec4& mountainColor)
{
    terrainProgram->setUniform("GrassColor", glm::vec3(grassColor) / 255.0f);
    terrainProgram->setUniform("SandColor", glm::vec3(sandColor) / 255.0f);
    terrainProgram->setUniform("MountainColor", glm::vec3(mountainColor) / 255.0f);
}

void Terrain::setClipPlane(const glm::vec4& plane)
{
    terrainProgram->setUniform("ClipPlane", plane);
    treesProgram->setUniform("ClipPlane", plane);
}

void Terrain::draw(const glm::mat4& projection, const glm::mat4& view, const Lighting& lighting)
{
    for (auto prog : { terrainProgram.get(), treesProgram.get() })
    {
        lighting.setLightParams(*prog, view);
        prog->setUniform("Projection", projection);
        prog->setUniform("View", view);
    }

    auto frustum = util::frustumPlanes(projection * view);

    terrainProgram->use();

    dirtTexture.bindTo(0);
    terrainProgram->setUniform("NoiseTexture", 0);
    terrainProgram->setUniform("UnitsPerPeriod", 32.0f);

    // Sort them from back to front
    std::vector<std::pair<float, gl::Mesh*>> meshesToDraw;
    meshesToDraw.reserve(terrainMeshes.size());
    for (std::size_t i = 0; i < terrainMeshes.size(); i++)
    {
        if (frustum.checkIntersectionAABB(terrainMin[i], terrainMax[i]))
            meshesToDraw.emplace_back(util::planeDistanceAABB(frustum.near, terrainMin[i], terrainMax[i]), &terrainMeshes[i]);
    }

    std::sort(meshesToDraw.begin(), meshesToDraw.end());
    for (auto& mesh : meshesToDraw) mesh.second->draw(glm::mat4(1.0));

    treesProgram->use();
    trunkMesh.draw(trunkInstances);
    coneMesh.draw(coneInstances);
}

float Terrain::operator()(float x, float z) const
{
    float i = x / resolution, j = -z / resolution;

    if (i < -xofs || j < -yofs || i > -xofs + heights.width() - 1 || j > -yofs + heights.height() - 1)
        return -std::numeric_limits<float>::infinity();

    // Get the integer (offsetted) and fractional parts
    ssize ti = std::floor(i), tj = std::floor(j);
    auto fi = i - ti, fj = j - tj;
    ti += xofs; tj += yofs;

    // Special cases for the end edges
    if (ti == heights.width() - 1 && tj == heights.height() - 1)
        return heights(ti, tj);
    else if (ti == heights.width() - 1)
        return (1 - fj) * heights(ti, tj) + fj * heights(ti, tj + 1);
    else if (tj == heights.height() - 1)
        return (1 - fi) * heights(ti, tj) + fi * heights(ti + 1, tj);
    else
    {
        // Follow the mesh topology, so not really a bilinear interpolation
        if (fi + fj <= 1.0f)
            return (1 - fi) * heights(ti, tj) + (fi - fj) * heights(ti + 1, tj) + fj * heights(ti + 1, tj + 1);
        else return (1 - fj) * heights(ti, tj) + (fj - fi) * heights(ti, tj + 1) + fi * heights(ti + 1, tj + 1);
    }
}
