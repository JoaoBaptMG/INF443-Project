#include "resources/FileUtils.hpp"

#include "Model.hpp"
#include "ModelUtils.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <algorithm>
#include <unordered_map>

namespace fs = std::filesystem;

void addNodeRecursive(const aiNode* node, std::unordered_map<const aiNode*, std::size_t>& nodeIndices)
{
    auto nextId = nodeIndices.size();
    nodeIndices[node] = nextId;

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        addNodeRecursive(node->mChildren[i], nodeIndices);
}

model::Model file_utils::loadModel(fs::path path)
{
    Assimp::Importer importer;

    auto str = path.u8string();
    auto scene = importer.ReadFile(str.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_SortByPType
        | aiProcess_Debone | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw model::ModelException("Error loading Assimp model: " + std::string(importer.GetErrorString()));

    model::Model model;

    // First, process all the model's meshes
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        model.addMesh(scene->mMeshes[i]);

    // Then the model's materials
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
        model.addMaterial(path, scene, scene->mMaterials[i]);

    // Then the animations
    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
        model.addAnimation(scene->mAnimations[i]);

    // Find the nodes and do a topological sort on them
    std::unordered_map<const aiNode*, std::size_t> nodeIndices;
    addNodeRecursive(scene->mRootNode, nodeIndices);
    model.addNodes(std::move(nodeIndices));

    // Then finally fix the animation references
    model.fixAnimationReferences();

    // It's finished
    return model;
}