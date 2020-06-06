#include "Shader.hpp"

#include "wrappers/glException.hpp"

#include <iostream>

using namespace gl;

auto shaderTypeToString(ShaderType type)
{
    switch (type)
    {
    case ShaderType::VertexShader: return "vertex";
    case ShaderType::GeometryShader: return "geometry";
    case ShaderType::FragmentShader: return "fragment";
    }
    return "";
}

Shader::Shader(ShaderType type, const char* source)
{
    if (type == ShaderType::Unknown) throw ShaderException("Cannot create shader of Unknown type!");

    shader = glCreateShader(static_cast<GLenum>(type));
   

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check if compilation was okay
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) throw ShaderException(std::string("Failed to compile ") + shaderTypeToString(type) + " shader: " + getInfoLog());
    std::cout << "This " << shaderTypeToString(type) << " shader info log: " << getInfoLog() << std::endl;
}

void Shader::setName(const std::string& name) const
{
    glObjectLabelKHR(GL_SHADER, shader, name.size(), name.data());
}

std::string Shader::getInfoLog() const
{
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::string infoLog(length, 0);
    glGetShaderInfoLog(shader, length, nullptr, &infoLog[0]);
    return infoLog;
}

ShaderType Shader::getType() const
{
    GLint type;
    glGetShaderiv(shader, GL_SHADER_TYPE, &type);
    return static_cast<ShaderType>(type);
}

Shader::~Shader()
{
    glDeleteShader(shader);
}
