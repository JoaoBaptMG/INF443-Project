#pragma once

#include "resources/Framebuffer.hpp"
#include "resources/Texture.hpp"
#include "resources/Renderbuffer.hpp"
#include "resources/Mesh.hpp"
#include "resources/Program.hpp"
#include "resources/Query.hpp"
#include <queue>

#include <glm/glm.hpp>
#include "wrappers/glfw.hpp"

namespace scene
{
    class Water final
    {
        // All the coordinates
        float y;
        float xmin, zmin;
        float xmax, zmax;
        float time;

        // Water offset velocities
        glm::vec2 waterVelocities[2];

        // The framebuffer definition
        struct Framebuffers
        {
            gl::Framebuffer framebuffer;
            gl::Texture2D color;
            gl::Renderbuffer depth;
        };

        // The reflection and refraction framebuffers
        Framebuffers reflection, refraction;

        // The mesh and the program
        gl::Mesh waterMesh;
        gl::Texture2D rippleTextures[2];

        // Generate a set of occlusion queries
        std::queue<gl::Query> queries;
        bool lastOcclusionValue;

        std::shared_ptr<gl::Program> waterProgram;
        std::shared_ptr<gl::Program> queryProgram;

        void generateRipple(int seed);

    public:
        Water() = default;
        Water(float y, float xmin, float zmin, float xmax, float zmax, glfw::Size fbsize, int seed);
        void recreateAttachments(glfw::Size fbsize);

        // The reflection step
        glm::mat4 getReflectionMatrix() const;
        glm::vec4 getReflectionClipPlane() const;
        void beginReflection();
        void endReflection();

        // The refraction step
        glm::vec4 getRefractionClipPlane() const;
        void beginRefraction();
        void endRefraction();

        void update(double delta) { time += delta; }
        void draw(const glm::mat4& projection, const glm::mat4& view);

        // Put an occlusion query here to optimize results
        void checkOcclusion(const glm::mat4& projection, const glm::mat4& view);
        bool shouldDraw();
    };
}
