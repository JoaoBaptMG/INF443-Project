#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstddef>

namespace gl
{
    class InstanceSet final
    {
        GLuint matrixBuffer;
        GLsizei numInstances;

    public:
        InstanceSet() : numInstances(0) { glGenBuffers(1, &matrixBuffer); }
        ~InstanceSet() { glDeleteBuffers(1, &matrixBuffer); }

        // Disallow copying
        InstanceSet(const InstanceSet&) = delete;
        InstanceSet& operator=(const InstanceSet&) = delete;

        // Enable moving
        InstanceSet(InstanceSet&& o) noexcept : numInstances(o.numInstances), matrixBuffer(o.matrixBuffer) { o.matrixBuffer = 0; }
        InstanceSet& operator=(InstanceSet&& o) noexcept
        {
            numInstances = o.numInstances;
            std::swap(matrixBuffer, o.matrixBuffer);
            return *this;
        }

        // Upload the instances
        void setInstances(const std::vector<glm::mat4>& matrices)
        {
            glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * matrices.size(), matrices.data(), GL_STREAM_DRAW);
            numInstances = matrices.size();
        }

        // Use them
        void useInstances() const
        {
            glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);

            for (int i = 0; i < 4; i++)
            {
                glEnableVertexAttribArray(4 + i);
                glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
                glVertexAttribDivisor(4 + i, 1); // This is what sets it instanced
            }
        }

        friend class Mesh;
    };
}
