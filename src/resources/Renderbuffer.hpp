#pragma once

#include <glad/glad.h>
#include <algorithm>
#include "TextureFormats.hpp"

namespace gl
{
    class Renderbuffer final
    {
        GLuint renderbuffer;
        static inline GLuint lastBoundRenderbuffer = 0;

    public:
        Renderbuffer() { glGenRenderbuffers(1, &renderbuffer); }
        ~Renderbuffer() { glDeleteRenderbuffers(1, &renderbuffer); }

        // Disallow copying
        Renderbuffer(const Renderbuffer&) = delete;
        Renderbuffer& operator=(const Renderbuffer&) = delete;

        // Enable moving
        Renderbuffer(Renderbuffer&& o) noexcept : renderbuffer(o.renderbuffer) { o.renderbuffer = 0; }
        Renderbuffer& operator=(Renderbuffer&& o) noexcept
        {
            std::swap(renderbuffer, o.renderbuffer);
            return *this;
        }

        void setName(const std::string& name)
        {
            glObjectLabelKHR(GL_RENDERBUFFER, renderbuffer, name.size(), name.data());
        }

        void bind() const 
        { 
            if (lastBoundRenderbuffer != renderbuffer)
            {
                glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
                lastBoundRenderbuffer = renderbuffer;
            }
        }

        void storage(gl::InternalFormat format, GLsizei width, GLsizei height)
        {
            bind();
            glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(format), width, height);
        }

        friend class Framebuffer;
    };
}
