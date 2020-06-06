#pragma once

namespace util
{
    template <typename Key, auto Value> struct choice;

    template <typename Key, typename VType, typename... Choices> struct choice_list {};

    template <typename Key, typename VType, auto Value, typename... NextChoices>
    struct choice_list<Key, VType, choice<Key, Value>, NextChoices...>
    {
        static constexpr auto value = static_cast<VType>(Value);
    };

    template <typename Key, typename VType, typename Choice, typename... NextChoices>
    struct choice_list<Key, VType, Choice, NextChoices...> : choice_list<Key, VType, NextChoices...> {};
}
