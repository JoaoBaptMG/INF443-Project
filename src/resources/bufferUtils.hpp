#pragma once

#include <glad/glad.h>
#include "wrappers/glParamFromType.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace gl
{
    template <typename T>
    struct vector_traits
    {
        static constexpr std::size_t size = T::length();
        using type = typename T::value_type;
    };

    template <>
    struct vector_traits<int>
    {
        static constexpr std::size_t size = 1;
        using type = int;
    };

    template <>
    struct vector_traits<float>
    {
        static constexpr std::size_t size = 1;
        using type = float;
    };

    template <typename T>
    GLuint createAndFillBuffer(const T* data, std::size_t size, GLenum target = GL_ARRAY_BUFFER)
    {
        if (data == nullptr || size == 0) return 0;
        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(target, buffer);
        glBufferData(target, size * sizeof(T), data, GL_STATIC_DRAW);
        return buffer;
    }

    template <typename T>
    GLuint createAndConfigureVertexArray(const T* data, std::size_t size, GLuint index, bool normalized = false)
    {
        if (index == -1 || data == nullptr || size == 0) return 0;
        auto buffer = createAndFillBuffer(data, size);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, vector_traits<T>::size, ParamFromType<vector_traits<T>::type>, normalized, sizeof(T), nullptr);

        return buffer;
    }

    template <typename T>
    GLuint createAndConfigureVertexArrayInteger(const T* data, std::size_t size, GLuint index)
    {
        if (index == -1 || data == nullptr || size == 0) return 0;
        auto buffer = createAndFillBuffer(data, size);
        glEnableVertexAttribArray(index);
        glVertexAttribIPointer(index, vector_traits<T>::size, ParamFromType<vector_traits<T>::type>, sizeof(T), nullptr);

        return buffer;
    }

    template <typename T>
    GLuint createAndFillBuffer(const std::vector<T>& data, GLenum target = GL_ARRAY_BUFFER)
    {
        return createAndFillBuffer<T>(data.data(), data.size(), target);
    }

    template <typename T>
    GLuint createAndConfigureVertexArray(const std::vector<T>& data, GLuint index, bool normalized = false)
    {
        return createAndConfigureVertexArray(data.data(), data.size(), index, normalized);
    }

    template <typename T>
    GLuint createAndConfigureVertexArrayInteger(const std::vector<T>& data, GLuint index)
    {
        return createAndConfigureVertexArrayInteger(data.data(), data.size(), index);
    }
}
