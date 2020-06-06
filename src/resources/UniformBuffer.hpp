#pragma once

#include <glad/glad.h>
#include <algorithm>
#include <string>

namespace gl
{
    class UniformBuffer final
    {
        GLuint buffer;
        inline static thread_local GLuint lastBufferBound = 0;

    public:
        UniformBuffer() { glGenBuffers(1, &buffer); }
        ~UniformBuffer() { glDeleteBuffers(1, &buffer); }

        // Disallow copying
        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;

        // Enable moving
        UniformBuffer(UniformBuffer&& o) noexcept : buffer(o.buffer) { o.buffer = 0; }
        UniformBuffer& operator=(UniformBuffer&& o) noexcept
        {
            std::swap(buffer, o.buffer);
            return *this;
        }

        void setName(const std::string& name)
        {
            glObjectLabelKHR(GL_BUFFER, buffer, name.size(), name.data());
        }

        void bind() const
        {
            if (lastBufferBound != buffer)
            {
                glBindBuffer(GL_UNIFORM_BUFFER, buffer);
                lastBufferBound = buffer;
            }
        }

        void bindTo(GLuint index) const
        {
            glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer);
            lastBufferBound = 0;
        }

        void upload(const void* data, GLsizeiptr size)
        {
            bind(); glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
        }

        template <typename T, std::size_t N>
        void upload(const T val[N]) { upload(val, sizeof(T) * N); }
    };
}
