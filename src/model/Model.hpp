#pragma once

#include <stdexcept>

#include "resources/Program.hpp"
#include "resources/Texture.hpp"
#include <memory>
#include <unordered_map>
#include "ModelMesh.hpp"
#include "ModelMaterial.hpp"
#include "ModelAnimation.hpp"
#include "assimp/scene.h"
#include <filesystem>

namespace model
{
    class ModelException final : public std::runtime_error
    {
    public:
        ModelException(std::string what) : std::runtime_error(what) {}
    };

    class Model final
    {
        std::vector<ModelMesh> meshes;
        std::vector<ModelMaterial> materials;
        std::unordered_map<std::string, std::shared_ptr<gl::Texture2D>> embeddedTextures;

        std::vector<std::size_t> nodeParents;
        std::vector<glm::mat4> nodeRelativeTransforms;
        std::vector<unsigned int> nodeMeshIndices;
        std::vector<std::size_t> nodeMeshStarts;
        std::vector<std::size_t> nodeNumMeshes;
        std::vector<std::string> nodeNames;
        std::unordered_map<std::string, std::size_t> nodesByName;

        glm::mat4 globalInverseTransform;
        std::vector<ModelAnimation> animations;
        std::size_t curAnimation;

    public:
        Model();
        void addMesh(const aiMesh* mesh) { meshes.emplace_back(*program, mesh); }
        void addMaterial(std::filesystem::path oldPath, const aiScene* scene, const aiMaterial* material)
        {
            materials.emplace_back(oldPath, *this, scene, material);
        }
        void addAnimation(const aiAnimation* anim) { animations.emplace_back(anim); }

        std::shared_ptr<gl::Texture2D> addTexture(std::string name, const aiTexture* texture);
        void addNodes(std::unordered_map<const aiNode*, std::size_t> nodeIndices);

        void fixAnimationReferences();
        void setAnimation(std::string name);

        void setTime(double time);
        void draw(const glm::mat4& model);

        std::shared_ptr<gl::Program> program;
    };
}