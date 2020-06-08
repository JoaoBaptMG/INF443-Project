#pragma once

#include <functional>
#include <filesystem>
#include "util/generic_shared_ptr.hpp"
#include "Program.hpp"

namespace cache
{
    using CacheLoader = std::function<util::generic_shared_ptr(std::filesystem::path)>;

    void addLoader(std::string extension, CacheLoader loader);

    util::generic_shared_ptr load(std::filesystem::path path);

    template <typename T>
    inline std::shared_ptr<T> load(std::filesystem::path path) { return load(path).as<T>(); }

    std::shared_ptr<gl::Program> loadProgram(std::initializer_list<std::filesystem::path> shaders);

    void clear();
}
