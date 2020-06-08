#include "Mesh.hpp"

#include "wrappers/glException.hpp"
#include "bufferUtils.hpp"
#include <algorithm>
#include <numeric>

using namespace gl;

template <typename T>
static void concat(std::vector<T>& out, const std::vector<T>& in1, std::size_t expSize1, const std::vector<T>& in2, std::size_t expSize2)
{
    if (in1.empty() && in2.empty()) out.clear();
    else
    {
        out.resize(expSize1 + expSize2);
        auto it = std::copy(in1.begin(), in1.end(), out.begin());
        std::copy(in2.begin(), in2.end(), out.begin() + expSize1);
    }
}

MeshBuilder gl::operator+(const MeshBuilder& mb1, const MeshBuilder& mb2)
{
    MeshBuilder mesh;

    auto expSize1 = mb1.validateAndGetNumberOfVertices();
    auto expSize2 = mb2.validateAndGetNumberOfVertices();

    bool hasHomogeneous1 = mb1.positions.empty() && !mb1.positionsH.empty();
    bool hasHomogeneous2 = mb2.positions.empty() && !mb2.positionsH.empty();

    // Convert homogeneous whether necessary
    if (!hasHomogeneous1 && !hasHomogeneous2)
        concat(mesh.positions, mb1.positions, expSize1, mb2.positions, expSize2);
    else if (hasHomogeneous1 && !hasHomogeneous2)
    {
        auto toVec4 = [](const glm::vec3& vec) { return glm::vec4(vec, 1); };
        std::vector<glm::vec4> homogeneous2(mb2.positions.size());
        std::transform(mb2.positions.begin(), mb2.positions.end(), homogeneous2.begin(), toVec4);
        concat(mesh.positionsH, mb1.positionsH, expSize1, homogeneous2, expSize2);
    }
    else if (!hasHomogeneous1 && hasHomogeneous2)
    {
        auto toVec4 = [](const glm::vec3& vec) { return glm::vec4(vec, 1); };
        std::vector<glm::vec4> homogeneous1(mb1.positions.size());
        std::transform(mb1.positions.begin(), mb1.positions.end(), homogeneous1.begin(), toVec4);
        concat(mesh.positionsH, homogeneous1, expSize1, mb2.positionsH, expSize2);
    }
    else concat(mesh.positionsH, mb1.positionsH, expSize1, mb2.positionsH, expSize2);

    // Concat the remaining of the attributes
    concat(mesh.normals, mb1.normals, expSize1, mb2.normals, expSize2);
    concat(mesh.colors, mb1.colors, expSize1, mb2.colors, expSize2);
    concat(mesh.texcoords, mb1.texcoords, expSize1, mb2.texcoords, expSize2);

    if (!mb1.indices.empty() || !mb2.indices.empty())
    {
        auto numElements1 = mb1.indices.empty() ? expSize1 : mb1.indices.size();
        auto numElements2 = mb2.indices.empty() ? expSize2 : mb2.indices.size();
        concat(mesh.indices, mb1.indices, numElements1, mb2.indices, numElements2);

        if (mb1.indices.empty()) std::iota(mesh.indices.begin(), mesh.indices.begin() + numElements1, 0);

        if (mb2.indices.empty()) std::iota(mesh.indices.begin() + numElements1, mesh.indices.end(), expSize1);
        else std::for_each(mesh.indices.begin() + numElements1, mesh.indices.end(), [=](auto& idx) { idx += (unsigned int)expSize1; });
    }

    return mesh;
}

std::size_t MeshBuilder::validateAndGetNumberOfVertices() const
{
    // Either you define normal coordinates or homogeneous coordinates, not both
    if (!positions.empty() && !positionsH.empty())
        throw MeshException("Cannot define positions and homogeneous positions at the same time!");

    auto sizes = { positions.size(), positionsH.size(), normals.size(), colors.size(), texcoords.size() };

    // First, ensure the consistency of the meshBuilder
    auto expectedSize = std::max(sizes);
    for (auto size : sizes)
        if (size != 0 && size != expectedSize)
            throw MeshException("Inconsistent MeshBuilder size when building!");

    return expectedSize;
}

MeshBuilder& MeshBuilder::operator+=(const MeshBuilder& other)
{
    *this = *this + other;
    return *this;
}

Mesh::Mesh(const MeshBuilder& meshBuilder, PrimitiveType primitiveType) : primitiveType(primitiveType)
{
    auto numVertices = meshBuilder.validateAndGetNumberOfVertices();
   
    // Generate the vertex array and bind the necessary indices
    glGenVertexArrays(1, &vertexArray); 

    // And bind the vertex array
    glBindVertexArray(vertexArray); 

    // Generate and configure the attributes
    if (meshBuilder.positionsH.empty())
        positionBuffer = createAndConfigureVertexArray(meshBuilder.positions, 0);
    else positionBuffer = createAndConfigureVertexArray(meshBuilder.positionsH, 0);
    normalBuffer = createAndConfigureVertexArray(meshBuilder.normals, 1);
    colorBuffer = createAndConfigureVertexArray(meshBuilder.colors, 2, true);
    texcoordBuffer = createAndConfigureVertexArray(meshBuilder.texcoords, 3);

    // Build the index list
    elementBuffer = createAndFillBuffer(meshBuilder.indices, GL_ELEMENT_ARRAY_BUFFER);
    numElements = (unsigned int)(meshBuilder.indices.empty() ? numVertices : meshBuilder.indices.size());

    // Unbind the vertex array
    glBindVertexArray(0);
}

Mesh Mesh::empty()
{
    // Create an empty (but valid) mesh
    Mesh mesh;
    glGenVertexArrays(1, &mesh.vertexArray);
    mesh.primitiveType = PrimitiveType::Triangles;
    return mesh;
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept
{
    std::swap(vertexArray, mesh.vertexArray);
    std::swap(numElements, mesh.numElements);
    std::swap(primitiveType, mesh.primitiveType);
    std::swap(elementBuffer, mesh.elementBuffer);
    std::swap(positionBuffer, mesh.positionBuffer);
    std::swap(normalBuffer, mesh.normalBuffer);
    std::swap(colorBuffer, mesh.colorBuffer);
    std::swap(texcoordBuffer, mesh.texcoordBuffer);
    return *this;
}

void Mesh::setBufferName(GLuint buffer, std::string name)
{
    if (buffer) glObjectLabelKHR(GL_BUFFER, buffer, name.size(), name.data());
}

void Mesh::setName(const std::string& name)
{
    glObjectLabelKHR(GL_VERTEX_ARRAY, vertexArray, name.size(), name.data());
    setBufferName(positionBuffer, name + " - position");
    setBufferName(normalBuffer, name + " - normal");
    setBufferName(colorBuffer, name + " - color");
    setBufferName(texcoordBuffer, name + " - texcoord");
    setBufferName(elementBuffer, name + " - elements");
}

template <typename T>
void refillBufferStream(GLuint& buffer, const std::vector<T>& data, GLenum target = GL_ARRAY_BUFFER)
{
    if (buffer == 0 && !data.empty())
    {
        glGenBuffers(1, &buffer);
    }
    else if (buffer != 0 && data.empty())
    {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }

    if (data.empty()) return;
    glBindBuffer(target, buffer);
    glBufferData(target, data.size() * sizeof(T), data.data(), GL_STREAM_DRAW);
}

template <typename T>
void reconfigureVertexArrayStream(GLuint& buffer, const std::vector<T>& data, GLuint index, bool normalized = false)
{
    refillBufferStream(buffer, data);

    if (buffer != 0)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, vector_traits<T>::size, ParamFromType<typename vector_traits<T>::type>, normalized, sizeof(T), nullptr);
    }
    else glDisableVertexAttribArray(index);
}

void Mesh::streamMesh(const MeshBuilder& meshBuilder, PrimitiveType newPrimitiveType)
{
    auto numVertices = meshBuilder.validateAndGetNumberOfVertices();

    // Bind the vertex array
    glBindVertexArray(vertexArray);

    // Recreate all buffers
    if (meshBuilder.positionsH.empty())
        reconfigureVertexArrayStream(positionBuffer, meshBuilder.positions, 0);
    else reconfigureVertexArrayStream(positionBuffer, meshBuilder.positionsH, 0);
    reconfigureVertexArrayStream(normalBuffer, meshBuilder.normals, 1);
    reconfigureVertexArrayStream(colorBuffer, meshBuilder.colors, 2, true);
    reconfigureVertexArrayStream(texcoordBuffer, meshBuilder.texcoords, 3);

    // Rebuild the index list
    refillBufferStream(elementBuffer, meshBuilder.indices, GL_ELEMENT_ARRAY_BUFFER);
    numElements = (unsigned int)(meshBuilder.indices.empty() ? numVertices : meshBuilder.indices.size());
    primitiveType = newPrimitiveType;

    // Unbind it in order to avoid outside changes
    glBindVertexArray(0);
}

void Mesh::draw(const glm::mat4& model) const
{
    if (numElements == 0) return;

    // Bind the vertex array
    glBindVertexArray(vertexArray);

    // Bind the vertex attribute
    glVertexAttrib4fv(4, glm::value_ptr(model[0]));
    glVertexAttrib4fv(5, glm::value_ptr(model[1]));
    glVertexAttrib4fv(6, glm::value_ptr(model[2]));
    glVertexAttrib4fv(7, glm::value_ptr(model[3]));

    // Use the appropriate draw function
    auto mode = static_cast<GLenum>(primitiveType);
    if (elementBuffer) glDrawElements(mode, numElements, GL_UNSIGNED_INT, nullptr);
    else glDrawArrays(mode, 0, numElements);
}

void Mesh::draw(const InstanceSet& instances) const
{
    if (numElements == 0) return;

    // Bind the vertex array
    glBindVertexArray(vertexArray);

    // Bind the vertex attribute
    instances.useInstances();

    // Use the appropriate draw function
    auto mode = static_cast<GLenum>(primitiveType);
    if (elementBuffer) glDrawElementsInstanced(mode, numElements, GL_UNSIGNED_INT, nullptr, instances.numInstances);
    else glDrawArraysInstanced(mode, 0, numElements, instances.numInstances);
}


Mesh::~Mesh()
{
    // Delete the vertex array
    glDeleteVertexArrays(1, &vertexArray);

    glDeleteBuffers(1, &elementBuffer);
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &texcoordBuffer);
}
