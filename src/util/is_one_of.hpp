#pragma once

#include <type_traits>

namespace util
{
    template <typename TVal, typename... TChoices>
    struct is_one_of_type : std::bool_constant<(std::is_same_v<TVal, TChoices> || ...)> {};

    template <typename TVal, typename... TChoices>
    constexpr auto is_one_of_type_v = is_one_of_type<TVal, TChoices...>::value;

    template <auto Val, auto... Choices>
    struct is_one_of_value : std::bool_constant<((Val == Choices) || ...)> {};

    template <auto Val, auto... Choices>
    constexpr auto is_one_of_value_v = is_one_of_value<Val, Choices...>::value;
}
