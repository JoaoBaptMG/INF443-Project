#include "Model.hpp"
#include "ModelUtils.hpp"

#include "resources/Cache.hpp"

#include <memory>
#include <glm/gtc/type_ptr.hpp>

using namespace model;

Model::Model()
{
    program = cache::loadProgram({
        "resources/shaders/lighting.frag",
        "resources/shaders/model.vert",
        "resources/shaders/model.frag",
    });
    program->setName("Model Program");

    curAnimation = 0;
}

std::shared_ptr<gl::Texture2D> Model::addTexture(std::string name, const aiTexture* texture)
{
    // First, check if it is already here
    auto it = embeddedTextures.find(name);

    if (it == embeddedTextures.end())
    {
        // If the texture is compressed, decompress it
        if (texture->mHeight == 0)
            it = embeddedTextures.emplace(name, std::make_shared<gl::Texture2D>(
                loadCompressedImageFromMemory(reinterpret_cast<const unsigned char*>(texture->pcData), 
                    texture->mWidth))).first;
        else // The format is BGRA, so load it accordingly
            it = embeddedTextures.emplace(name, std::make_shared<gl::Texture2D>(
                loadImageFromMemory(reinterpret_cast<const unsigned char*>(texture->pcData),
                    texture->mWidth, texture->mHeight, gl::Format::BGRA))).first;
    }

    return it->second;
}

void Model::addNodes(std::unordered_map<const aiNode*, std::size_t> nodeIndices)
{
    // Allocate space for all the nodes
    nodeParents.resize(nodeIndices.size());
    nodeRelativeTransforms.resize(nodeIndices.size());
    nodeMeshStarts.resize(nodeIndices.size());
    nodeNumMeshes.resize(nodeIndices.size());
    nodeNames.resize(nodeIndices.size());

    // Set the parent as the first
    nodeIndices[nullptr] = -1;

    for (auto [node, idx] : nodeIndices)
    {
        if (!node) continue;

        // The nodeIndices is topologically sorted
        nodeParents[idx] = nodeIndices[node->mParent];

        // There's a quirk here: Assimp transforms are always row-major, and
        // we expect column major matrices, so we should transpose
        nodeRelativeTransforms[idx] = toGlm(node->mTransformation);
        if (node->mParent == nullptr) globalInverseTransform = glm::inverse(nodeRelativeTransforms[idx]);

        // Set the node names for animations
        nodesByName[node->mName.C_Str()] = idx;
        nodeNames[idx] = node->mName.C_Str();

        // Finally, we should store the mesh indices
        nodeMeshStarts[idx] = nodeMeshIndices.size();
        nodeNumMeshes[idx] = node->mNumMeshes;

        auto oldSize = nodeMeshIndices.size();
        nodeMeshIndices.resize(oldSize + node->mNumMeshes);
        std::copy_n(node->mMeshes, node->mNumMeshes, nodeMeshIndices.begin() + oldSize);
    }
}

void Model::fixAnimationReferences()
{
    // First, for each mesh, we are going to attribute ids to each bone
    for (auto& mesh : meshes)
    {
        auto numBones = mesh.boneNames.size();
        for (std::size_t i = 0; i < numBones; i++)
            mesh.boneNodeIndices[i] = nodesByName[mesh.boneNames[i]];
        mesh.boneNames.clear();
    }
}

void Model::setAnimation(std::string name)
{
    curAnimation = std::find_if(animations.begin(), animations.end(),
        [&](const ModelAnimation& animation) { return animation.getName() == name; }) - animations.begin();
}

void Model::setTime(double time)
{
    // For each node that has a non-null animation, interpolate it
    for (std::size_t i = 0; i < nodeParents.size(); i++)
        animations[curAnimation].transformInterpolateChannel(nodeRelativeTransforms[i], time, nodeNames[i]);
}

void Model::draw(const glm::mat4& model)
{
    // Create the absolute node transforms
    std::vector<glm::mat4> nodeBaseTransforms(nodeParents.size());
    for (std::size_t i = 0; i < nodeParents.size(); i++)
    {
        if (nodeParents[i] == -1) nodeBaseTransforms[i] = glm::mat4(1.0);
        else nodeBaseTransforms[i] = nodeBaseTransforms[nodeParents[i]] * nodeRelativeTransforms[i];
    }

    // Now, draw the meshes
    program->use();
    for (std::size_t i = 0; i < nodeParents.size(); i++)
    {
        auto idx = nodeMeshStarts[i];
        auto len = nodeNumMeshes[i];

        for (std::size_t j = idx; j < idx + len; j++)
        {
            auto& mesh = meshes[nodeMeshIndices[j]];
            auto& material = materials[mesh.materialIndex];
            
            // Set the material attributes
            if (material.diffuseTexture)
            {
                material.diffuseTexture->bindTo(0);
                program->setUniform("DiffuseTexture", 0);
            }

            if (material.specularTexture)
            {
                material.specularTexture->bindTo(1);
                program->setUniform("SpecularTexture", 1);
            }

            program->setUniform("Shininess", material.shininess);

            // Set the bone transforms
            for (unsigned int k = 0; k < mesh.boneNodeIndices.size(); k++)
            {
                std::string name = "Bones[" + std::to_string(k) + ']';
                auto transform = nodeBaseTransforms[mesh.boneNodeIndices[k]] * mesh.boneMatrices[k];
                program->setUniform(name.c_str(), globalInverseTransform * transform);
            }

            mesh.draw(model * nodeBaseTransforms[i]);
        }
    }
}
