#pragma once

#include <assimp/mesh.h>
#include <glad/glad.h>
#include "resources/Program.hpp"
#include "resources/Mesh.hpp"
#include <vector>

namespace model
{
    class ModelMesh final
    {
        GLuint vertexArray;

        GLuint positionBuffer;
        GLuint normalBuffer;
        std::vector<GLuint> colorBuffers;
        std::vector<GLuint> texcoordBuffers;
        GLuint boneBuffer;
        GLuint weightBuffer;
        GLuint elementBuffer;
        GLuint numElements;
        gl::PrimitiveType primitiveType;
        unsigned int materialIndex;

        std::vector<glm::mat4> boneMatrices;
        std::vector<std::string> boneNames;
        std::vector<std::size_t> boneNodeIndices;

        void configureAnimationBones(const aiMesh* mesh, const gl::Program& program);

    public:
        ModelMesh(const gl::Program& program, const aiMesh* mesh = nullptr);
        ~ModelMesh();

        // Disallow copying
        ModelMesh(const ModelMesh&) = delete;
        ModelMesh& operator=(const ModelMesh&) = delete;

        // Enable moving
        ModelMesh(ModelMesh&& o) noexcept;
        ModelMesh& operator=(ModelMesh&& o) noexcept;

        // Draw
        void draw(const glm::mat4& model) const;

        friend class Model;
    };
}
