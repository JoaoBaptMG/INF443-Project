#include "FileUtils.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>
#include <vector>
#include "util/string_utils.hpp"
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#include "Cache.hpp"

using namespace std::literals::string_view_literals;
namespace fs = std::filesystem;

auto nextToken(const std::string_view& str, std::size_t val = 0)
{
    auto nextNonSpace = str.find_first_not_of(" \t\r\n", val);
    auto nextSpace = str.find_first_of(" \t\r\n", nextNonSpace);
    return str.substr(nextNonSpace, nextSpace - nextNonSpace);
}

auto nextQuotes(const std::string_view& str, std::size_t val = 0)
{
    auto nextNonSpace = str.find_first_not_of(" \t\r\n", val);
    if (nextNonSpace == std::string_view::npos || str[nextNonSpace] != '"') 
        return std::string_view();
    
    auto nextQuotes = str.find_first_of("\"", nextNonSpace + 1);
    return str.substr(nextNonSpace + 1, nextQuotes - nextNonSpace - 1);
}

gl::Shader file_utils::loadShader(fs::path path, gl::ShaderType type)
{
    std::vector<fs::path> outPaths;

    try
    {
        // The output stream
        std::ostringstream output;

        std::stack<std::ifstream> streams;
        std::stack<std::size_t> pathIds;
        std::stack<std::size_t> lineNumbers;

        std::vector<fs::path> paths;

        // Create the first ifstream
        std::ifstream first(path);
        if (!first) throw gl::ShaderException("Unable to open file " + path.u8string());

        paths.push_back(path);

        // Now, cycle through all streams
        streams.push(std::move(first));
        pathIds.push(0);
        lineNumbers.push(0);
        while (!streams.empty())
        {
            auto& stream = streams.top();
            auto& lnumber = lineNumbers.top();
            auto& pathId = pathIds.top();

            if (lnumber != 0 || pathId != 0) output << "#line " << (lnumber + 1) << ' ' << pathId << '\n';

            std::string line;
            while (std::getline(stream, line))
            {
                lnumber++;
                auto linev = std::string_view(line);

                // Check user-defined pragmas
                if (util::starts_with(linev, "#type"))
                {
                    // Type-defining pragma
                    auto val = nextToken(linev, sizeof("#type") - 1);
                    gl::ShaderType target;

                    if (val == "vertex") target = gl::ShaderType::VertexShader;
                    else if (val == "geometry") target = gl::ShaderType::GeometryShader;
                    else if (val == "fragment") target = gl::ShaderType::FragmentShader;
                    else throw gl::ShaderException("Unknown shader type declaration!");

                    if (type == gl::ShaderType::Unknown) type = target;
                    else if (type != target) throw gl::ShaderException("Inconsistent shader type declarations!");

                    output << '\n';
                }
                else if (util::starts_with(linev, "#include"))
                {
                    if (streams.size() == 256)
                        throw gl::ShaderException("Too many nested includes!");

                    // Include
                    auto val = nextQuotes(linev, sizeof("#include") - 1);
                    if (val.empty()) throw gl::ShaderException("Invalid value for include!");

                    // Compute the path
                    auto nextPath = path.parent_path() / val;

                    // Open the file
                    std::ifstream next(nextPath);
                    if (!next) throw gl::ShaderException("Unable to open file " + nextPath.u8string());

                    paths.push_back(nextPath);
                    streams.push(std::move(next));
                    pathIds.push(paths.size());
                    lineNumbers.push(0);
                    
                    // And include the path in the normal file
                    break;
                }
                else
                {
                    // Just a normal line
                    output << linev << '\n';
                }
            }

            // Pop everything
            if (stream.eof())
            {
                streams.pop();
                pathIds.pop();
                lineNumbers.pop();
            }
        }

        std::swap(paths, outPaths);

        auto str = output.str();
        gl::Shader shader(type, str.c_str());
        shader.setName(path.filename().u8string());
        return shader;
    }
    catch (gl::ShaderException exc)
    {
        std::ostringstream what;
        what << "Error occured while parsing file " << path << ":\n" << exc.what();

        if (!outPaths.empty())
        {
            what << "\nList of paths:";
            std::size_t i = 0;
            for (const auto& path : outPaths)
                what << "\n" << (i++) << ": " << path;
        }

        throw gl::ShaderException(what.str());
    }
}

gl::Texture2D file_utils::loadImage(fs::path path)
{
    int width, height, numChannels;
    auto str = path.u8string();
    unsigned char* data = stbi_load(str.c_str(), &width, &height, &numChannels, 4);
    if (!data) throw LoadException("Error loading image: " + std::string(stbi_failure_reason()));

    gl::Texture2D texture;
    texture.assign(0, gl::InternalFormat::RGBA8, width, height, gl::Format::RGBA, data);
    texture.generateMipmap();
    texture.setMagFilter(gl::MagFilter::Linear);
    texture.setMinFilter(gl::MinFilter::LinearMipLinear);

    stbi_image_free(data);
    return texture;
}

void file_utils::addDefaultLoaders()
{
    cache::addLoader(".vert", [](fs::path path) 
        { return std::make_shared<gl::Shader>(loadShader(path, gl::ShaderType::VertexShader)); });
    cache::addLoader(".geom", [](fs::path path) 
        { return std::make_shared<gl::Shader>(loadShader(path, gl::ShaderType::GeometryShader)); });
    cache::addLoader(".frag", [](fs::path path) 
        { return std::make_shared<gl::Shader>(loadShader(path, gl::ShaderType::FragmentShader)); });
    cache::addLoader(".png", [](fs::path path)
        { return std::make_shared<gl::Texture2D>(loadImage(path)); });
    cache::addLoader(".jpg", [](fs::path path)
        { return std::make_shared<gl::Texture2D>(loadImage(path)); });
    cache::addLoader(".jpeg", [](fs::path path)
        { return std::make_shared<gl::Texture2D>(loadImage(path)); });
    cache::addLoader(".gltf", [](fs::path path) 
        { return std::make_shared<model::Model>(loadModel(path)); });
}
