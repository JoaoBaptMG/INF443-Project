#pragma once

#include <glad/glad.h>
#include <algorithm>

#include "Texture.hpp"
#include "Renderbuffer.hpp"
#include "util/is_one_of.hpp"

namespace gl
{
    enum class FramebufferStatus : GLenum
    {
        Complete = GL_FRAMEBUFFER_COMPLETE,
        Undefined = GL_FRAMEBUFFER_UNDEFINED,
        Unsupported = GL_FRAMEBUFFER_UNSUPPORTED,
        IncompleteAttachment = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
        IncompleteMissingAttachment = GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
        IncompleteDrawBuffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
        IncompleteReadBuffer = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
        IncompleteMultisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
        IncompleteLayerTargets = GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,
    };

    class Attachment final
    {
        GLenum attachment;

        constexpr Attachment(GLenum attachment) : attachment(attachment) {}
    public:
        friend class Framebuffer;
    };

    class Framebuffer final
    {
        GLuint framebuffer;
        static inline thread_local GLuint lastBoundFramebuffer = 0;

        explicit Framebuffer(int) : framebuffer(0) {};
    public:
        static Framebuffer none() { return Framebuffer(-1); }

        Framebuffer() { glGenFramebuffers(1, &framebuffer); }
        ~Framebuffer() { glDeleteFramebuffers(1, &framebuffer); }

        // Disallow copying
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        // Enable moving
        Framebuffer(Framebuffer&& o) noexcept : framebuffer(o.framebuffer) { o.framebuffer = 0; }
        Framebuffer& operator=(Framebuffer&& o) noexcept
        {
            std::swap(framebuffer, o.framebuffer);
            return *this;
        }

        void setName(const std::string& name)
        {
            glObjectLabelKHR(GL_FRAMEBUFFER, framebuffer, name.size(), name.data());
        }

        void bind() const
        { 
            if (lastBoundFramebuffer != framebuffer)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
                lastBoundFramebuffer = framebuffer;
            }
        }

        static void bindDefault()
        { 
            if (lastBoundFramebuffer != 0)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                lastBoundFramebuffer = 0;
            }
        }

        template <GLenum Target>
        void attach(Attachment attachment, const Texture<Target>& tex, GLint level = 0)
        {
            bind(); tex.bind();
            glFramebufferTexture(GL_FRAMEBUFFER, attachment.attachment, tex.texture, level);
        }

        template <GLenum Target>
        void attachLayer(Attachment attachment, const Texture<Target>& tex, GLint level, GLint layer)
        {
            bind(); tex.bind();
            glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment.attachment, tex.texture, level, layer);
        }

        void attach(Attachment attachment, const Renderbuffer& rb)
        {
            bind(); rb.bind();
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment.attachment, GL_RENDERBUFFER, rb.renderbuffer);
        }

        auto getStatus() const { bind(); return static_cast<FramebufferStatus>(glCheckFramebufferStatus(GL_FRAMEBUFFER)); }

        static constexpr Attachment ColorAttachment(GLenum i) { return GL_COLOR_ATTACHMENT0 + i; }
        static constexpr Attachment DepthAttachment = GL_DEPTH_ATTACHMENT;
        static constexpr Attachment StencilAttachment = GL_STENCIL_ATTACHMENT;
        static constexpr Attachment DepthStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
    };
}
