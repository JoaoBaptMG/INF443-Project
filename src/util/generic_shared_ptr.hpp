#pragma once

#include <memory>
#include <string>
#include <stdexcept>
#include <typeindex>

namespace util
{
    class type_mismatch : public std::runtime_error
    {
    public:
        const char* type_name;

        type_mismatch(const char* type_name, std::string text)
            : std::runtime_error(text), type_name(type_name) {}

        template <typename T>
        static type_mismatch for_type()
        {
            return type_mismatch(typeid(T).name(),
                std::string("Attempt to cast an incompatible generic_shared_ptr "
                    "to type ") + typeid(T).name());
        }
    };

    using type_id_t = std::type_index;

    template <typename T>
    static type_id_t type_id() { return std::type_index(typeid(T)); }

    static const type_id_t notype = type_id<void>();

    class generic_shared_ptr
    {
        type_id_t type;
        std::shared_ptr<void> ptr;

    public:
        generic_shared_ptr() : type(notype), ptr() {}

        template <typename T>
        generic_shared_ptr(std::shared_ptr<T> other) noexcept
            : type(type_id<T>()), ptr(std::move(other)) {}

        friend inline void swap(generic_shared_ptr& p1, generic_shared_ptr& p2) noexcept
        {
            using std::swap;
            swap(p1.type, p2.type);
            swap(p1.ptr, p2.ptr);
        }

        inline void reset() noexcept { type = notype; ptr.reset(); }
        inline auto use_count() const noexcept { return ptr.use_count(); }
        inline explicit operator bool() const noexcept { return type != notype && static_cast<bool>(ptr); }

        template <typename T>
        bool is() const noexcept { return type == type_id<T>(); }
        bool empty() const noexcept { return type == notype; }

        template <typename T>
        auto try_convert() const noexcept
        {
            if (is<T>()) return std::static_pointer_cast<T>(ptr);
            else return std::shared_ptr<T>();
        }

        template <typename T>
        std::shared_ptr<T> as() const
        {
            if (auto p = try_convert<T>()) return p;
            else throw type_mismatch::for_type<T>();
        }

        template <typename T>
        inline explicit operator std::shared_ptr<T>() const { return as<T>(); }
    };
}
