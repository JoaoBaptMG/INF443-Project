#include "ModelMaterial.hpp"

#include "Model.hpp"
#include <filesystem>
#include <iostream>
#include "resources/Cache.hpp"

using namespace model;
namespace fs = std::filesystem;

ModelMaterial::ModelMaterial(fs::path oldPath, Model& model, const aiScene* scene, const aiMaterial* material)
{
    // Pick up a single diffuse and a single specular texture
    aiString path;
    
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
    {
        // If the texture is an embedded texture, add it to the model
        if (auto texture = scene->GetEmbeddedTexture(path.C_Str()))
            diffuseTexture = model.addTexture(path.C_Str(), texture);
        else
        {
            // Else, build a path from it
            auto newPath = oldPath.parent_path() / path.C_Str();
            diffuseTexture = cache::load<gl::Texture2D>(newPath);
        }
    }

    // Same thing for specular texture
    if (material->GetTexture(aiTextureType_SPECULAR, 0, &path) == aiReturn_SUCCESS)
    {
        if (auto texture = scene->GetEmbeddedTexture(path.C_Str()))
            specularTexture = model.addTexture(path.C_Str(), texture);
        else
        {
            auto newPath = oldPath.parent_path() / path.C_Str();
            specularTexture = cache::load<gl::Texture2D>(newPath);
        }
    }

    // Get the shininess factor
    material->Get(AI_MATKEY_SHININESS, shininess);
}
