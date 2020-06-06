#pragma once

#include <glad/glad.h>
#include <type_traits>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Shader.hpp"

namespace gl
{
    class ProgramException : public std::runtime_error
    {
    public:
        ProgramException(std::string what) : std::runtime_error(what) {}
    };

    class Program
    {
        static thread_local GLuint lastUsedProgram;
        GLuint program;

        void relink();

    public:
        Program() : program(0) {}

        template <typename... Params>
        Program(const Params&... params) : program(glCreateProgram())
        {
            static_assert((std::is_same_v<Params, Shader> && ...));
            (glAttachShader(program, params.shader), ...);
            relink();
        }

        Program(const std::vector<std::shared_ptr<Shader>>& shaders) : program(glCreateProgram())
        {
            for (auto shader : shaders)
                glAttachShader(program, shader->shader);
            relink();
        }

        // Disallow copying
        Program(const Program&) = delete;
        Program& operator=(const Program&) = delete;

        // Enable moving
        Program(Program&& o) noexcept : program(o.program) { o.program = 0; }
        Program& operator=(Program&& o) noexcept
        {
            std::swap(program, o.program);
            return *this;
        }

        void use() const;
        bool isValid() const;
        std::string getInfoLog() const;

        void setName(const std::string& name) const;

        // Attribute and uniform data
        auto getAttributeLocation(const char* name) const { return glGetAttribLocation(program, name); }
        auto getUniformLocation(const char* name) const { return glGetUniformLocation(program, name); }
        auto getUniformBlockIndex(const char* name) const { return glGetUniformBlockIndex(program, name); }

        // All uniform setting functons
        void setUniform(const char* name, float value);
        void setUniform(const char* name, const glm::vec1& value);
        void setUniform(const char* name, const glm::vec2& value);
        void setUniform(const char* name, const glm::vec3& value);
        void setUniform(const char* name, const glm::vec4& value);
        void setUniform(const char* name, int value);
        void setUniform(const char* name, const glm::ivec1& value);
        void setUniform(const char* name, const glm::ivec2& value);
        void setUniform(const char* name, const glm::ivec3& value);
        void setUniform(const char* name, const glm::ivec4& value);
        void setUniform(const char* name, unsigned int value);
        void setUniform(const char* name, const glm::uvec1& value);
        void setUniform(const char* name, const glm::uvec2& value);
        void setUniform(const char* name, const glm::uvec3& value);
        void setUniform(const char* name, const glm::uvec4& value);
        void setUniform(const char* name, const glm::mat2& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat3& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat4& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat2x3& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat3x2& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat2x4& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat4x2& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat3x4& value, bool transpose = false);
        void setUniform(const char* name, const glm::mat4x3& value, bool transpose = false);

        // Vector uniform setting functions
        void setUniform(const char* name, const std::vector<float>& value);
        void setUniform(const char* name, const std::vector<glm::vec1>& value);
        void setUniform(const char* name, const std::vector<glm::vec2>& value);
        void setUniform(const char* name, const std::vector<glm::vec3>& value);
        void setUniform(const char* name, const std::vector<glm::vec4>& value);
        void setUniform(const char* name, const std::vector<int>& value);
        void setUniform(const char* name, const std::vector<glm::ivec1>& value);
        void setUniform(const char* name, const std::vector<glm::ivec2>& value);
        void setUniform(const char* name, const std::vector<glm::ivec3>& value);
        void setUniform(const char* name, const std::vector<glm::ivec4>& value);
        void setUniform(const char* name, const std::vector<unsigned int>& value);
        void setUniform(const char* name, const std::vector<glm::uvec1>& value);
        void setUniform(const char* name, const std::vector<glm::uvec2>& value);
        void setUniform(const char* name, const std::vector<glm::uvec3>& value);
        void setUniform(const char* name, const std::vector<glm::uvec4>& value);
        void setUniform(const char* name, const std::vector<glm::mat2>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat3>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat4>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat2x3>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat3x2>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat2x4>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat4x2>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat3x4>& value, bool transpose = false);
        void setUniform(const char* name, const std::vector<glm::mat4x3>& value, bool transpose = false);

        void bindUniformBlock(const char* name, int index);

        // Destructor
        ~Program();
    };
}
