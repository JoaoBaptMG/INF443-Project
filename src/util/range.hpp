#pragma once

#include <iterator>
#include <type_traits>

namespace util
{
    template <typename T>
    class range_iterator final
    {
        static_assert(std::is_integral_v<T>, "range_iterator must be used with integral types");
        T idx;

    public:
        using difference_type = std::make_signed_t<T>;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::random_access_iterator_tag;

        range_iterator(T v = 0) noexcept : idx(v) {}

        // Increment and decrement
        range_iterator& operator++() noexcept { ++idx; return *this; }
        range_iterator operator++(int) noexcept { return range_iterator(idx++); }
        range_iterator& operator--() noexcept { --idx; return *this; }
        range_iterator operator--(int) noexcept { return range_iterator(idx--); }

        // Dereference
        const T& operator*() const noexcept { return idx; }

        // Addition, subtraction, array index
        range_iterator& operator+=(difference_type n) noexcept { idx += n; return *this; }
        range_iterator& operator-=(difference_type n) noexcept { idx -= n; return *this; }
        
        friend range_iterator operator+(const range_iterator& it, difference_type n) noexcept
        {
            return range_iterator(it.idx + n);
        }

        friend range_iterator operator+(difference_type n, const range_iterator& it) noexcept
        {
            return range_iterator(n + it.idx);
        }

        friend range_iterator operator-(const range_iterator& it, difference_type n) noexcept
        {
            return range_iterator(it.idx - n);
        }

        friend difference_type operator-(const range_iterator& it1, const range_iterator& it2) noexcept
        {
            return it1.idx - it2.idx;
        }

        const T& operator[](difference_type n) const noexcept { return idx + n; }

        // Comparison
        friend bool operator==(const range_iterator& it1, const range_iterator& it2) noexcept { return it1.idx == it2.idx; }
        friend bool operator!=(const range_iterator& it1, const range_iterator& it2) noexcept { return it1.idx != it2.idx; }
        friend bool operator<(const range_iterator& it1, const range_iterator& it2) noexcept { return it1.idx < it2.idx; }
        friend bool operator>(const range_iterator& it1, const range_iterator& it2) noexcept { return it1.idx > it2.idx; }
        friend bool operator<=(const range_iterator& it1, const range_iterator& it2) noexcept { return it1.idx <= it2.idx; }
        friend bool operator>=(const range_iterator& it1, const range_iterator& it2) noexcept { return it1.idx >= it2.idx; }
    };

    template <typename T>
    class range final
    {
        static_assert(std::is_integral_v<T>, "range_iterator must be used with integral types");
        T _begin, _end;

    public:
        range(T begin, T end) noexcept : _begin(begin), _end(end) {}

        auto begin() const noexcept { return range_iterator(_begin); }
        auto cbegin() const noexcept { return range_iterator(_begin); }

        auto end() const noexcept { return range_iterator(_end); }
        auto cend() const noexcept { return range_iterator(_end); }

        auto rbegin() const noexcept { return std::make_reverse_iterator(begin()); }
        auto crbegin() const noexcept { return std::make_reverse_iterator(cbegin()); }

        auto rend() const noexcept { return std::make_reverse_iterator(end()); }
        auto crend() const noexcept { return std::make_reverse_iterator(cend()); }
    };
}
