#pragma once

#include "resources/bufferUtils.hpp"
#include <assimp/mesh.h>

namespace gl
{
    template <typename T>
    struct vector_traits<aiVector2t<T>>
    {
        static constexpr auto size = 2;
        using type = T;
    };

    template <typename T>
    struct vector_traits<aiVector3t<T>>
    {
        static constexpr auto size = 3;
        using type = T;
    };

    template <>
    struct vector_traits<aiColor3D>
    {
        static constexpr auto size = 3;
        using type = ai_real;
    };

    template <typename T>
    struct vector_traits<aiColor4t<T>>
    {
        static constexpr auto size = 4;
        using type = T;
    };
}
