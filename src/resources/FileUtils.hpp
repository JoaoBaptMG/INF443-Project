#pragma once

#include "Shader.hpp"
#include "Texture.hpp"
#include "model/Model.hpp"
#include <filesystem>
#include <stdexcept>

namespace file_utils
{
    class LoadException : public std::runtime_error
    {
    public:
        LoadException(std::string what) : std::runtime_error(what) {}
    };

    gl::Shader loadShader(std::filesystem::path path, gl::ShaderType type = gl::ShaderType::Unknown);
    gl::Texture2D loadImage(std::filesystem::path);
    model::Model loadModel(std::filesystem::path path);

    void addDefaultLoaders();
}
