#include "ModelUtils.hpp"
#include "Model.hpp"

#define STBI_FAILURE_USERMSG
#include "stb_image.h"

gl::Texture2D model::loadImageFromMemory(const unsigned char* data, GLsizei width, GLsizei height, gl::Format format)
{
    gl::Texture2D texture;

    texture.assign(0, gl::InternalFormat::RGBA8, width, height, format, data);
    texture.generateMipmap();
    texture.setMagFilter(gl::MagFilter::Linear);
    texture.setMinFilter(gl::MinFilter::LinearMipLinear);

    return texture;
}

gl::Texture2D model::loadCompressedImageFromMemory(const unsigned char* data, std::size_t size)
{
    int width, height, numChannels;
    unsigned char* imgdata = stbi_load_from_memory(data, size, &width, &height, &numChannels, 4);
    if (!imgdata) throw ModelException("Error loading image: " + std::string(stbi_failure_reason()));
    auto texture = loadImageFromMemory(imgdata, width, height);
    stbi_image_free(imgdata);
    return texture;
}
