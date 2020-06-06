#pragma once

#include "resources/Texture.hpp"
#include <cstddef>
#include <glm/glm.hpp>
#include <assimp/matrix4x4.h>

namespace model
{
    template <typename T>
    inline static glm::tmat4x4<T> toGlm(const aiMatrix4x4t<T>& matrix)
    {
        // Assimp matrix is always row-major, where we use column-major matrices, so we should transpose
        return glm::tmat4x4<T>(
            glm::tvec4<T>(matrix.a1, matrix.b1, matrix.c1, matrix.d1),
            glm::tvec4<T>(matrix.a2, matrix.b2, matrix.c2, matrix.d2),
            glm::tvec4<T>(matrix.a3, matrix.b3, matrix.c3, matrix.d3),
            glm::tvec4<T>(matrix.a4, matrix.b4, matrix.c4, matrix.d4));
    }

    gl::Texture2D loadImageFromMemory(const unsigned char* data, GLsizei width, GLsizei height,
        gl::Format format = gl::Format::RGBA);
    gl::Texture2D loadCompressedImageFromMemory(const unsigned char* data, std::size_t size);
}