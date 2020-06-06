#pragma once

#include <assimp/scene.h>
#include <memory>
#include <filesystem>
#include "resources/Texture.hpp"

namespace model
{
    class Model;

    class ModelMaterial final
    {
    public:
        ModelMaterial(std::filesystem::path oldPath, Model& model, const aiScene* scene, const aiMaterial* material);
        // It's a material containing diffuse texture and specular texture only,
        // so everything else can be default

        std::shared_ptr<gl::Texture2D> diffuseTexture;
        std::shared_ptr<gl::Texture2D> specularTexture;
        float shininess;
    };
}