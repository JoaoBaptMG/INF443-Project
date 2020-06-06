#include "Program.hpp"

#include "wrappers/glException.hpp"
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace gl;

thread_local GLuint Program::lastUsedProgram = 0;

void Program::relink()
{
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) throw ProgramException("Failed to link program: " + getInfoLog());
    std::cout << "Program info log: " << getInfoLog() << std::endl;
}

void Program::use() const
{
    if (lastUsedProgram != program)
    {
        glUseProgram(program);
        lastUsedProgram = program;
    }
}

bool Program::isValid() const
{
    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    return status;
}

std::string Program::getInfoLog() const
{
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::string infoLog(length, 0);
    glGetProgramInfoLog(program, length, nullptr, &infoLog[0]);
    return infoLog;
}

void Program::setName(const std::string& name) const
{
    glObjectLabelKHR(GL_PROGRAM, program, name.size(), name.data());
}

Program::~Program()
{
    glDeleteProgram(program);
}

void Program::setUniform(const char* name, float value)
{
    use(); glUniform1f(getUniformLocation(name), value);
}

void Program::setUniform(const char* name, const glm::vec1& value)
{
    use(); glUniform1f(getUniformLocation(name), value.x);
}

void Program::setUniform(const char* name, const glm::vec2& value)
{
    use(); glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::vec3& value)
{
    use(); glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::vec4& value)
{
    use(); glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, int value)
{
    use(); glUniform1i(getUniformLocation(name), value);
}

void Program::setUniform(const char* name, const glm::ivec1& value)
{
    use(); glUniform1i(getUniformLocation(name), value.x);
}

void Program::setUniform(const char* name, const glm::ivec2& value)
{
    use(); glUniform2iv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::ivec3& value)
{
    use(); glUniform3iv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::ivec4& value)
{
    use(); glUniform4iv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, unsigned int value)
{
    use(); glUniform1ui(getUniformLocation(name), value);
}

void Program::setUniform(const char* name, const glm::uvec1& value)
{
    use(); glUniform1ui(getUniformLocation(name), value.x);
}

void Program::setUniform(const char* name, const glm::uvec2& value)
{
    use(); glUniform2uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::uvec3& value)
{
    use(); glUniform3uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::uvec4& value)
{
    use(); glUniform4uiv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat2& value, bool transpose)
{
    use(); glUniformMatrix2fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat3& value, bool transpose)
{
    use(); glUniformMatrix3fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat4& value, bool transpose)
{
    use(); glUniformMatrix4fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat2x3& value, bool transpose)
{
    use(); glUniformMatrix2x3fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat3x2& value, bool transpose)
{
    use(); glUniformMatrix3x2fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat2x4& value, bool transpose)
{
    use(); glUniformMatrix2x4fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat4x2& value, bool transpose)
{
    use(); glUniformMatrix4x2fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat3x4& value, bool transpose)
{
    use(); glUniformMatrix3x4fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const glm::mat4x3& value, bool transpose)
{
    use(); glUniformMatrix4x3fv(getUniformLocation(name), 1, transpose, glm::value_ptr(value));
}

void Program::setUniform(const char* name, const std::vector<float>& value)
{
    use(); glUniform1fv(getUniformLocation(name), (GLsizei)value.size(), value.data());
}

void Program::setUniform(const char* name, const std::vector<glm::vec1>& value)
{
    use(); glUniform1fv(getUniformLocation(name), (GLsizei)value.size(), &value[0].x);
}

void Program::setUniform(const char* name, const std::vector<glm::vec2>& value)
{
    use(); glUniform2fv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::vec3>& value)
{
    use(); glUniform3fv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::vec4>& value)
{
    use(); glUniform4fv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<int>& value)
{
    use(); glUniform1iv(getUniformLocation(name), (GLsizei)value.size(), value.data());
}

void Program::setUniform(const char* name, const std::vector<glm::ivec1>& value)
{
    use(); glUniform1iv(getUniformLocation(name), (GLsizei)value.size(), &value[0].x);
}

void Program::setUniform(const char* name, const std::vector<glm::ivec2>& value)
{
    use(); glUniform2iv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::ivec3>& value)
{
    use(); glUniform3iv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::ivec4>& value)
{
    use(); glUniform4iv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<unsigned int>& value)
{
    use(); glUniform1uiv(getUniformLocation(name), (GLsizei)value.size(), value.data());
}

void Program::setUniform(const char* name, const std::vector<glm::uvec1>& value)
{
    use(); glUniform1uiv(getUniformLocation(name), (GLsizei)value.size(), &value[0].x);
}

void Program::setUniform(const char* name, const std::vector<glm::uvec2>& value)
{
    use(); glUniform2uiv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::uvec3>& value)
{
    use(); glUniform3uiv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::uvec4>& value)
{
    use(); glUniform4uiv(getUniformLocation(name), (GLsizei)value.size(), glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat2>& value, bool transpose)
{
    use(); glUniformMatrix2fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat3>& value, bool transpose)
{
    use(); glUniformMatrix3fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat4>& value, bool transpose)
{
    use(); glUniformMatrix4fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat2x3>& value, bool transpose)
{
    use(); glUniformMatrix2x3fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat3x2>& value, bool transpose)
{
    use(); glUniformMatrix3x2fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat2x4>& value, bool transpose)
{
    use(); glUniformMatrix2x4fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat4x2>& value, bool transpose)
{
    use(); glUniformMatrix4x2fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat3x4>& value, bool transpose)
{
    use(); glUniformMatrix3x4fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void Program::setUniform(const char* name, const std::vector<glm::mat4x3>& value, bool transpose)
{
    use(); glUniformMatrix4x3fv(getUniformLocation(name), (GLsizei)value.size(), transpose, glm::value_ptr(value[0]));
}

void gl::Program::bindUniformBlock(const char* name, int index)
{
    glUniformBlockBinding(program, getUniformBlockIndex(name), index);
}
