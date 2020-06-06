#pragma once

#include <type_traits>

namespace detail
{
    template <typename Enum>
    class flags_enum final
    {
        static_assert(std::is_enum_v<Enum>, "Use flags_enum only with enumerations!");

        using value_type = std::underlying_type_t<Enum>;
        value_type val;

        inline static constexpr auto to_val(Enum e) { return static_cast<value_type>(e); }

    public:
        constexpr explicit flags_enum(value_type val) : val(val) {}
        constexpr flags_enum(Enum e) : flags_enum(to_val(e)) {}

        constexpr explicit operator Enum() const { return static_cast<Enum>(val); }
        constexpr explicit operator bool() const { return val; }
        constexpr explicit operator value_type() const { return val; }

        constexpr flags_enum& operator|=(flags_enum o) { val |= o.val; return *this; }
        constexpr flags_enum& operator&=(flags_enum o) { val &= o.val; return *this; }
        constexpr flags_enum& operator^=(flags_enum o) { val ^= o.val; return *this; }

        constexpr flags_enum operator~() const { return flags_enum(~val); }

        constexpr bool operator==(flags_enum o) const { return val == o.val; }
        constexpr bool operator!=(flags_enum o) const { return val != o.val; }

        friend constexpr flags_enum operator|(flags_enum a, flags_enum b) { return flags_enum(a.val | b.val); }
        friend constexpr flags_enum operator&(flags_enum a, flags_enum b) { return flags_enum(a.val & b.val); }
        friend constexpr flags_enum operator^(flags_enum a, flags_enum b) { return flags_enum(a.val ^ b.val); }
    };
}

#define MAKE_FLAGS(Flags, Enum) \
    inline static constexpr ::detail::flags_enum<Enum> operator|(Enum a, Enum b)\
    { return ::detail::flags_enum<Enum>(a) | ::detail::flags_enum<Enum>(b); }\
    inline static constexpr ::detail::flags_enum<Enum> operator&(Enum a, Enum b)\
    { return ::detail::flags_enum<Enum>(a) & ::detail::flags_enum<Enum>(b); }\
    inline static constexpr ::detail::flags_enum<Enum> operator^(Enum a, Enum b)\
    { return ::detail::flags_enum<Enum>(a) ^ ::detail::flags_enum<Enum>(b); }\
    inline static constexpr ::detail::flags_enum<Enum> operator~(Enum a) { return ~::detail::flags_enum<Enum>(a); }\
    using Flags = ::detail::flags_enum<Enum>
