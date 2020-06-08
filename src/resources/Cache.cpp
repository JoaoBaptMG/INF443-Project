#include "Cache.hpp"

#include <unordered_map>
#include <map>
#include <sstream>

namespace fs = std::filesystem;
using namespace cache;

static std::unordered_map<std::string, CacheLoader> loaders;
static std::unordered_map<std::string, util::generic_shared_ptr> loadedAssets;

static std::unordered_map<std::string, std::shared_ptr<gl::Program>> programCache;

void cache::addLoader(std::string extension, CacheLoader loader)
{
    loaders.emplace(extension, loader);
}

static std::string pathExtension(fs::path path)
{
    auto str = path.u8string();
    auto val = str.find_last_of('.');
    return str.substr(val);
}

util::generic_shared_ptr cache::load(fs::path path)
{
    // First, try to locate it on the assets
    auto it = loadedAssets.find(path.u8string());
    if (it == loadedAssets.end())
    {
        // Else, try to load it
        auto lit = loaders.find(pathExtension(path));
        if (lit != loaders.end())
            it = loadedAssets.emplace(path.u8string(), lit->second(path)).first;
        else return util::generic_shared_ptr{};
    }

    return it->second;
}

std::shared_ptr<gl::Program> cache::loadProgram(std::initializer_list<fs::path> shaders)
{
    // build the key
    std::stringstream keyBuilder;
    for (const auto& path : shaders)
        keyBuilder << "////" << path.u8string();

    auto key = keyBuilder.str();

    // Try to locate on the program cache
    auto it = programCache.find(key);
    if (it == programCache.end())
    {
        std::vector<std::shared_ptr<gl::Shader>> shaderv;
        shaderv.reserve(shaders.size());

        for (const auto& path : shaders)
            shaderv.push_back(load<gl::Shader>(path));

        it = programCache.emplace(key, std::make_shared<gl::Program>(shaderv)).first;
    }

    return it->second;
}

void cache::clear()
{
    loaders.clear();
    loadedAssets.clear();
    programCache.clear();
}


