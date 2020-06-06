#pragma once

#include <glad/glad.h>

namespace gl
{
    enum class ComparisonFunction : GLint
    {
        LessEqual = GL_LEQUAL,
        GreaterEqual = GL_GEQUAL,
        Less = GL_LESS,
        Greater = GL_GREATER,
        Equal = GL_EQUAL,
        NotEqual = GL_NOTEQUAL,
        Always = GL_ALWAYS,
        Never = GL_NEVER
    };
}
