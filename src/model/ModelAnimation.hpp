#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <utility>
#include <unordered_map>
#include <assimp/anim.h>

namespace model
{
    struct NodeChannel final
    {
        std::vector<std::pair<double, glm::vec3>> positions;
        std::vector<std::pair<double, glm::quat>> rotations;
        std::vector<std::pair<double, glm::vec3>> scales;

        NodeChannel(const aiNodeAnim* anim);
    };

    class ModelAnimation final
    {
        double duration;
        double ticksPerSecond;
        std::unordered_map<std::string, NodeChannel> channels;
        std::string name;

    public:
        ModelAnimation(const aiAnimation* anim);
        bool transformInterpolateChannel(glm::mat4& target, double t, const std::string& nodeName) const;
        const std::string& getName() const { return name; }
    };
}