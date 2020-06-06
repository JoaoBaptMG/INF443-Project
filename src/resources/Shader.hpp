#pragma once

#include "glad/glad.h"
#include <utility>
#include <string>
#include <stdexcept>

namespace gl
{
    enum class ShaderType : GLenum
    {
        Unknown = 0,
        VertexShader = GL_VERTEX_SHADER,
        GeometryShader = GL_GEOMETRY_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER
    };

    class ShaderException : public std::runtime_error
    {
    public:
        ShaderException(std::string what) : std::runtime_error(what) {}
    };

    // Represents a class to encapsulate an OpenGL shader
    class Shader final
    {
        GLuint shader;

    public:
        Shader() = default;
        explicit Shader(ShaderType type, const char* source);

        // Disallow copying
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        // Enable moving
        Shader(Shader&& o) noexcept : shader(o.shader) { o.shader = 0; }
        Shader& operator=(Shader&& o) noexcept
        {
            std::swap(shader, o.shader);
            return *this;
        }

        void setName(const std::string& name) const;

        std::string getInfoLog() const;
        ShaderType getType() const;

        ~Shader();

        friend class Program;
    };
}
