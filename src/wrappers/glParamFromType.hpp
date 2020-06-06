#pragma once

#include <glad/glad.h>
#include "util/choice.hpp"
#include "util/is_one_of.hpp"

namespace gl
{
    template <typename T>
    constexpr auto ParamFromType = util::choice_list<T, GLenum,
        util::choice<float, GL_FLOAT>,
        util::choice<double, GL_DOUBLE>,
        util::choice<unsigned int, GL_UNSIGNED_INT>,
        util::choice<signed int, GL_INT>,
        util::choice<unsigned short, GL_UNSIGNED_SHORT>,
        util::choice<signed short, GL_SHORT>,
        util::choice<unsigned char, GL_UNSIGNED_BYTE>,
        util::choice<signed char, GL_BYTE>>::value;

    template <typename T>
    struct is_scalar : util::is_one_of_type<T, float, double, unsigned int,
        signed int, unsigned short, signed short, unsigned char, signed char> {};

    template <typename T>
    constexpr auto is_scalar_v = is_scalar<T>::value;
}