#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <type_traits>
#include <string>
#include "TextureFormats.hpp"
#include "wrappers/glParamFromType.hpp"
#include "wrappers/glDepthComparisonMode.hpp"

namespace gl
{
    constexpr GLint getDimensionsFrom(GLenum target)
    {
        switch (target)
        {
        case GL_TEXTURE_1D: return 1;
        case GL_TEXTURE_2D: return 2;
        case GL_TEXTURE_RECTANGLE: return 2;
        case GL_TEXTURE_CUBE_MAP: return 2;
        case GL_TEXTURE_1D_ARRAY: return 2;
        case GL_TEXTURE_3D: return 3;
        case GL_TEXTURE_2D_ARRAY: return 3;
        default: return -1;
        }
    }

    enum class FormatOptions : GLint
    {
        None, Integer, BGRA, BGRAInteger, Depth, DepthStencil
    };

    enum class MagFilter : GLint
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR
    };

    enum class MinFilter : GLint
    {
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR,
        NearestMipNearest = GL_NEAREST_MIPMAP_NEAREST,
        NearestMipLinear = GL_NEAREST_MIPMAP_LINEAR,
        LinearMipNearest = GL_LINEAR_MIPMAP_NEAREST,
        LinearMipLinear = GL_LINEAR_MIPMAP_LINEAR
    };

    enum class WrapEffect : GLint
    {
        Repeat = GL_REPEAT,
        MirroredRepeat = GL_MIRRORED_REPEAT,
        ClampToEdge = GL_CLAMP_TO_EDGE,
        ClampToBorder = GL_CLAMP_TO_BORDER
    };

    template <glm::length_t N>
    constexpr GLenum formatParam(FormatOptions options)
    {
        bool bgra = options == FormatOptions::BGRA || options == FormatOptions::BGRAInteger;
        bool integer = options == FormatOptions::Integer || options == FormatOptions::BGRAInteger;

        switch (N)
        {
        case 1: if (options == FormatOptions::Depth) return GL_DEPTH_COMPONENT;
            else return integer ? GL_RED_INTEGER : GL_RED;
        case 2: if (options == FormatOptions::DepthStencil) return GL_DEPTH_STENCIL; 
            else return integer ? GL_RG_INTEGER : GL_RG;
        case 3: return integer ? GL_RGB_INTEGER : GL_RGB;
        case 4: if (bgra) return integer ? GL_BGRA_INTEGER : GL_BGRA;
              else return integer ? GL_RGBA_INTEGER : GL_RGBA;
        default: return 0;
        }
    }

    // The format used if the pointer is null
    constexpr Format deriveDefaultFormat(InternalFormat internalFormat)
    {
        switch (internalFormat)
        {
        case InternalFormat::Depth:
        case InternalFormat::Depth16:
        case InternalFormat::Depth24:
        case InternalFormat::Depth32:
        case InternalFormat::Depth32f:
            return Format::Depth;
        case InternalFormat::DepthStencil:
            return Format::DepthStencil;
        default: return Format::Red;
        }
    }

    template <GLenum Target>
    class Texture final
    {
        GLuint texture;
        static inline thread_local GLuint lastBoundTexture = 0;

        Texture(int) : texture(0) {}
    public:
        constexpr static auto NumDimensions = getDimensionsFrom(Target);
        static_assert(NumDimensions != -1, "Invalid target passed to Texture!");

        static Texture none() { return Texture(-1); }
        Texture() { glGenTextures(1, &texture); }
        ~Texture() { glDeleteTextures(1, &texture); }

        // Disallow copying
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Enable moving
        Texture(Texture&& o) noexcept : texture(o.texture) { o.texture = 0; }
        Texture& operator=(Texture&& o) noexcept
        {
            std::swap(texture, o.texture);
            return *this;
        }

        void setName(const std::string& name)
        {
            glObjectLabelKHR(GL_TEXTURE, texture, name.size(), name.data());
        }

        void bind() const 
        { 
            if (lastBoundTexture != texture)
            {
                glBindTexture(Target, texture);
                lastBoundTexture = texture;
            }
        }

        void bindTo(GLuint unit) const
        { 
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(Target, texture);
            lastBoundTexture = 0;
        }

        void generateMipmap() { bind(); glGenerateMipmap(Target); }

        template <typename T>
        void assign(GLint level, InternalFormat internalFormat, GLsizei width, Format format, const T* data)
        {
            bind(); glTexImage1D(Target, level, static_cast<GLenum>(internalFormat), width, 
                0, static_cast<GLenum>(format), ParamFromType<T>, data);
        }

        void assign(GLint level, InternalFormat internalFormat, GLsizei width)
        {
            assign(level, internalFormat, width, deriveDefaultFormat(internalFormat), static_cast<const float*>(nullptr));
        }

        template <typename T>
        void assign(GLint level, InternalFormat internalFormat, GLsizei width, GLsizei height, Format format, const T* data)
        {
            bind(); glTexImage2D(Target, level, static_cast<GLenum>(internalFormat), width, height,
                0, static_cast<GLenum>(format), ParamFromType<T>, data);
        }

        void assign(GLint level, InternalFormat internalFormat, GLsizei width, GLsizei height)
        {
            assign(level, internalFormat, width, height, deriveDefaultFormat(internalFormat), static_cast<const float*>(nullptr));
        }

        template <typename T>
        void assign(GLint level, InternalFormat internalFormat, GLsizei width, GLsizei height, GLsizei depth, Format format, const T* data)
        {
            bind(); glTexImage3D(Target, level, static_cast<GLenum>(internalFormat), width, height, depth,
                0, static_cast<GLenum>(format), ParamFromType<T>, data);
        }

        void assign(GLint level, InternalFormat internalFormat, GLsizei width, GLsizei height, GLsizei depth)
        {
            assign(level, internalFormat, width, height, depth, deriveDefaultFormat(internalFormat), static_cast<const float*>(nullptr));
        }

        void setMagFilter(MagFilter filter) { bind(); glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter)); }
        void setMinFilter(MinFilter filter) { bind(); glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter)); }
        void setMaxAnisotropy(float f) { bind(); glTexParameterf(Target, GL_TEXTURE_MAX_ANISOTROPY_EXT, f); }

        void setWrapEffectS(WrapEffect effect) { bind(); glTexParameteri(Target, GL_TEXTURE_WRAP_S, static_cast<GLint>(effect)); }
        void setWrapEffectT(WrapEffect effect) { bind(); glTexParameteri(Target, GL_TEXTURE_WRAP_T, static_cast<GLint>(effect)); }
        void setWrapEffectR(WrapEffect effect) { bind(); glTexParameteri(Target, GL_TEXTURE_WRAP_R, static_cast<GLint>(effect)); }

        void setBorderColor(const glm::vec4& color) { bind(); glTexParameterfv(Target, GL_TEXTURE_BORDER_COLOR, &color.x); }

        void enableComparisonMode(ComparisonFunction func = ComparisonFunction::Less)
        {
            bind();
            glTexParameteri(Target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTexParameteri(Target, GL_TEXTURE_COMPARE_FUNC, static_cast<GLint>(func));
        }

        void clearComparisonMode() { bind(); glTexParameteri(Target, GL_TEXTURE_COMPARE_MODE, GL_NONE); }

        friend class Framebuffer;
    };

    using Texture1D = Texture<GL_TEXTURE_1D>;
    using Texture2D = Texture<GL_TEXTURE_2D>;
    using Texture3D = Texture<GL_TEXTURE_3D>;
    using TextureCubeMap = Texture<GL_TEXTURE_CUBE_MAP>;
    using TextureRectangle = Texture<GL_TEXTURE_RECTANGLE>;
    using Texture1DArray = Texture<GL_TEXTURE_1D_ARRAY>;
    using Texture2DArray = Texture<GL_TEXTURE_2D_ARRAY>;
}
