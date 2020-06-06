#include "ModelAnimation.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace model;

template <typename T>
glm::tvec3<T> toGlm(const aiVector3t<T>& vec)
{
    return glm::tvec3<T>(vec.x, vec.y, vec.z);
}

template <typename T>
glm::tquat<T> toGlm(const aiQuaterniont<T>& quat)
{
    return glm::tquat<T>(quat.w, quat.x, quat.y, quat.z);
}

template <typename T>
static auto getInterpolators(const std::vector<std::pair<double, T>>& val, double t)
{
    // First, do a upper_bound to pick up the best iterator
    auto it = std::upper_bound(val.begin(), val.end(), t,
        [](double t, const std::pair<double, T>& val) { return t < val.first; });

    // If the iterator is on the beginning, just return it
    if (it == val.begin()) return std::make_tuple(it->second, it->second, 0.0);

    // Else, if it is on the end, return the predecessor
    else if (it == val.end()) return std::make_tuple(it[-1].second, it[-1].second, 0.0);

    // Else, return the two interpolands
    else return std::make_tuple(it[-1].second, it->second, (t - it[-1].first) / (it->first - it[-1].first));
}

ModelAnimation::ModelAnimation(const aiAnimation* anim)
{
    // Load the duration and the ticks per second
    name = anim->mName.C_Str();
    duration = anim->mDuration;
    ticksPerSecond = anim->mTicksPerSecond;

    // Now, load each channel
    channels.reserve(anim->mNumChannels);
    for (unsigned int i = 0; i < anim->mNumChannels; i++)
        channels.emplace(anim->mChannels[i]->mNodeName.C_Str(), anim->mChannels[i]);
}

bool ModelAnimation::transformInterpolateChannel(glm::mat4& target, double t, const std::string& nodeName) const
{
    // Transform it into ticks
    t *= ticksPerSecond;
    t -= duration * std::floor(t / duration);

    // Find the character by name
    auto it = channels.find(nodeName);
    if (it != channels.end())
    {
        // Now interpolate the positions, rotations and scales
        const auto& channel = it->second;

        auto [pos1, pos2, f1] = getInterpolators(channel.positions, t);
        auto pos = glm::mix(pos1, pos2, f1);
        auto [quat1, quat2, f2] = getInterpolators(channel.rotations, t);
        auto quat = glm::slerp(quat1, quat2, (float)f2);
        auto [sca1, sca2, f3] = getInterpolators(channel.scales, t);
        auto scale = glm::mix(sca1, sca2, f3);

        // And compose the transformation
        target = glm::translate(pos) * glm::mat4_cast(quat) * glm::scale(scale);
        return true;
    }

    return false;
}

NodeChannel::NodeChannel(const aiNodeAnim* anim)
{
    // Populate the positions, rotations and scales
    positions.resize(anim->mNumPositionKeys);
    for (unsigned int i = 0; i < anim->mNumPositionKeys; i++)
    {
        positions[i].first = anim->mPositionKeys[i].mTime;
        positions[i].second = toGlm(anim->mPositionKeys[i].mValue);
    }

    rotations.resize(anim->mNumRotationKeys);
    for (unsigned int i = 0; i < anim->mNumRotationKeys; i++)
    {
        rotations[i].first = anim->mRotationKeys[i].mTime;
        rotations[i].second = toGlm(anim->mRotationKeys[i].mValue);
    }

    scales.resize(anim->mNumScalingKeys);
    for (unsigned int i = 0; i < anim->mNumScalingKeys; i++)
    {
        scales[i].first = anim->mScalingKeys[i].mTime;
        scales[i].second = toGlm(anim->mScalingKeys[i].mValue);
    }
}
