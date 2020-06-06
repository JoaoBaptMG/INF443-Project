#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include "InstanceSet.hpp"

namespace gl
{
    enum class PrimitiveType : GLenum
    {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineStrip = GL_LINE_STRIP,
        LineLoop = GL_LINE_LOOP,
        Triangles = GL_TRIANGLES,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleFan = GL_TRIANGLE_FAN
    };

    // The MeshBuilder is the structure that we'll feed to the Mesh constructor to actually build it
    struct MeshBuilder final
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec4> positionsH;
        std::vector<glm::vec3> normals;
        std::vector<glm::u8vec4> colors;
        std::vector<glm::vec2> texcoords;
        std::vector<unsigned int> indices;

        std::size_t validateAndGetNumberOfVertices() const;

        MeshBuilder& operator+=(const MeshBuilder& other);
    };

    MeshBuilder operator+(const MeshBuilder& mb1, const MeshBuilder& mb2);

    class MeshException final : public std::runtime_error
    {
    public:
        MeshException(std::string what) : std::runtime_error(what) {}
    };

    // The Mesh class represents the graphic object in the scene
    class Mesh final
    {
        GLuint vertexArray;
        unsigned int numElements;
        PrimitiveType primitiveType;

        GLuint elementBuffer;
        GLuint positionBuffer, normalBuffer, colorBuffer, texcoordBuffer;

    public:
        Mesh() noexcept : vertexArray(0), numElements(0), elementBuffer(0), positionBuffer(0), normalBuffer(0),
            colorBuffer(0), texcoordBuffer(0) {}
        Mesh(const MeshBuilder& meshBuilder, PrimitiveType primitiveType = PrimitiveType::Triangles);

        static Mesh empty();

        // Disable copying, enable moving
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& mesh) noexcept : Mesh() { *this = std::move(mesh); }
        Mesh& operator=(Mesh&& mesh) noexcept;

        void setBufferName(GLuint buffer, std::string name);
        void setName(const std::string& name);

        // reupload the data
        void streamMesh(const MeshBuilder& meshBuilder, PrimitiveType newPrimitiveType = PrimitiveType::Triangles);

        // draw the mesh
        void draw(const glm::mat4& modelMatrix) const;
        void draw(const InstanceSet& instances) const;

        // destructor
        ~Mesh();
    };
}