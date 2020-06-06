#pragma once

#include <string>
#include <string_view>

namespace util
{
    // Over-engineering string starts-with function
    template <typename CharT, typename Traits>
    static bool starts_with(const std::basic_string_view<CharT, Traits>& str, const std::basic_string_view<CharT, Traits>& val)
    {
        if (str.size() < val.size()) return false;
        return val == str.substr(0, val.size());
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool starts_with(const std::basic_string<CharT, Traits, Alloc>& str, const std::basic_string_view<CharT, Traits>& val)
    {
        return starts_with(std::basic_string_view<CharT, Traits>(str), val);
    }

    template <typename CharT, typename Traits>
    static bool starts_with(const CharT* str, const std::basic_string_view<CharT, Traits>& val)
    {
        return starts_with(std::basic_string_view<CharT, Traits>(str), val);
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool starts_with(const std::basic_string_view<CharT, Traits>& str, const std::basic_string<CharT, Traits, Alloc>& val)
    {
        return starts_with(str, std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits, typename Alloc1, typename Alloc2>
    static bool starts_with(const std::basic_string<CharT, Traits, Alloc1>& str, const std::basic_string<CharT, Traits, Alloc2>& val)
    {
        return starts_with(std::basic_string_view<CharT, Traits>(str), std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool starts_with(const CharT* str, const std::basic_string<CharT, Traits, Alloc>& val)
    {
        return starts_with(std::basic_string_view<CharT, Traits>(str), std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits>
    static bool starts_with(const std::basic_string_view<CharT, Traits>& str, const CharT* val)
    {
        return starts_with(str, std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool starts_with(const std::basic_string<CharT, Traits, Alloc>& str, const CharT* val)
    {
        return starts_with(std::basic_string_view<CharT, Traits>(str), std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT>
    static bool starts_with(const CharT* str, const CharT* val)
    {
        return starts_with(std::basic_string_view<CharT>(str), std::basic_string_view<CharT>(val));
    }

    // Over-engineering string ends-with function
    template <typename CharT, typename Traits>
    static bool ends_with(const std::basic_string_view<CharT, Traits>& str, const std::basic_string_view<CharT, Traits>& val)
    {
        if (str.size() < val.size()) return false;
        return val == str.substr(str.size() - val.size());
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool ends_with(const std::basic_string<CharT, Traits, Alloc>& str, const std::basic_string_view<CharT, Traits>& val)
    {
        return ends_with(std::basic_string_view<CharT, Traits>(str), val);
    }

    template <typename CharT, typename Traits>
    static bool ends_with(const CharT* str, const std::basic_string_view<CharT, Traits>& val)
    {
        return ends_with(std::basic_string_view<CharT, Traits>(str), val);
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool ends_with(const std::basic_string_view<CharT, Traits>& str, const std::basic_string<CharT, Traits, Alloc>& val)
    {
        return ends_with(str, std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits, typename Alloc1, typename Alloc2>
    static bool ends_with(const std::basic_string<CharT, Traits, Alloc1>& str, const std::basic_string<CharT, Traits, Alloc2>& val)
    {
        return ends_with(std::basic_string_view<CharT, Traits>(str), std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool ends_with(const CharT* str, const std::basic_string<CharT, Traits, Alloc>& val)
    {
        return ends_with(std::basic_string_view<CharT, Traits>(str), std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits>
    static bool ends_with(const std::basic_string_view<CharT, Traits>& str, const CharT* val)
    {
        return ends_with(str, std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT, typename Traits, typename Alloc>
    static bool ends_with(const std::basic_string<CharT, Traits, Alloc>& str, const CharT* val)
    {
        return ends_with(std::basic_string_view<CharT, Traits>(str), std::basic_string_view<CharT, Traits>(val));
    }

    template <typename CharT>
    static bool ends_with(const CharT* str, const CharT* val)
    {
        return ends_with(std::basic_string_view<CharT>(str), std::basic_string_view<CharT>(val));
    }
}