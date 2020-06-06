#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace util
{
	template <typename T>
	class array_view final
	{
		// The only important fields
		T* _arr;
		std::size_t _size;

	public:
		// The member types
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::make_signed_t<size_type>;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = std::reverse_iterator<pointer>;
		using const_reverse_iterator = std::reverse_iterator<const_pointer>;

		// Constructors and operators
		array_view() noexcept : array_view(0, nullptr) {}
		array_view(T* arr, size_type count) noexcept : _arr(arr), _size(count) {}
		array_view(const array_view& other) = default;
		array_view& operator=(const array_view& other) noexcept = default;

		// Checked access
		reference at(size_type position)
		{
			if (position >= _size) throw std::out_of_range("access to element " + std::to_string(position)
				+ " of an array view with " + std::to_string(_size) + " elements");
			return _arr[position];
		}

		const_reference at(size_type position) const
		{
			if (position >= _size) throw std::out_of_range("access to element " + std::to_string(position)
				+ " of an array view with " + std::to_string(_size) + " elements");
			return _arr[position];
		}

		// Unchecked access
		reference operator[](size_type position) noexcept { return _arr[position]; }
		const_reference operator[](size_type position) const noexcept { return _arr[position]; }

		// Other functions
		reference front() noexcept { return _arr[0]; }
		reference back() noexcept { return _arr[_size - 1]; }
		pointer data() noexcept { return _arr; }

		const_reference front() const noexcept { return _arr[0]; }
		const_reference back() const noexcept { return _arr[_size - 1]; }
		const_pointer data() const noexcept { return _arr; }

		// Iterators
		iterator begin() noexcept { return _arr; }
		iterator end() noexcept { return _arr + _size; }
		const_iterator begin() const noexcept { return _arr; }
		const_iterator end() const noexcept { return _arr + _size; }
		const_iterator cbegin() const noexcept { return _arr; }
		const_iterator cend() const noexcept { return _arr + _size; }

		// Reverse iterators
		reverse_iterator rbegin() noexcept { return reverse_iterator(_arr + _size - 1); }
		reverse_iterator rend() noexcept { return reverse_iterator(_arr - 1); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(_arr + _size - 1); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(_arr - 1); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(_arr + _size - 1); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(_arr - 1); }

		// Utilty functions
		bool empty() const noexcept { return _size == 0; }
		bool size() const noexcept { return _size; }

		// Swap
		void swap(array_view& s2) noexcept
		{
			using std::swap;
			swap(_size, s2._size);
			swap(_arr, s2._arr);
		}
	};

	// Comparison operators
	template <typename T>
	bool operator==(const array_view<T>& lhs, const array_view<T>& rhs)
	{
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template <typename T>
	bool operator!=(const array_view<T>& lhs, const array_view<T>& rhs) { return !(lhs == rhs); }

	template <typename T>
	bool operator<(const array_view<T>& lhs, const array_view<T>& rhs)
	{
		if (lhs.size() < rhs.size()) return true;
		if (lhs.size() > rhs.size()) return false;
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <typename T>
	bool operator>(const array_view<T>& lhs, const array_view<T>& rhs) { return rhs < lhs; }

	template <typename T>
	bool operator<=(const array_view<T>& lhs, const array_view<T>& rhs) { return !(rhs < lhs); }

	template <typename T>
	bool operator>=(const array_view<T>& lhs, const array_view<T>& rhs) { return !(lhs < rhs); }
}

// Specialization of std::swap
namespace std
{
	template <typename T>
	void swap(util::array_view<T>& lhs, util::array_view<T>& rhs) noexcept { lhs.swap(rhs); }
}
