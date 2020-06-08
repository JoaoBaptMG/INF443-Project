#include "Camera.hpp"

#include <glm/gtx/transform.hpp>

using namespace scene;

constexpr float PixelsPerFullRotation = 450;
constexpr float MoveSpeed = 10.0f;
constexpr float Pi = 3.14159265359f;

Camera::Camera(glfw::Window& window, float zFar) : position(), angles()
{
    // Disable the cursor for this window, in order to take control of the camera
    window.setCursorMode(glfw::CursorMode::Disabled);
    if (glfw::isRawMouseMotionSupported())
        window.setRawMouseMotionEnabled();

    lastPos = window.getCursorPos();

    auto fsize = window.getFramebufferSize();
    projection = glm::perspective(glm::radians(45.0f), (float)fsize.width / fsize.height, 0.5f, zFar);
    infiniteProjection = glm::infinitePerspective(glm::radians(45.0f), (float)fsize.width / fsize.height, 0.5f);
}

void Camera::update(glfw::Window& window, double delta)
{
    auto pos = window.getCursorPos();
    float dx = pos.x - lastPos.x;
    float dy = pos.y - lastPos.y;

    angles.x -= dx / PixelsPerFullRotation;
    angles.y -= dy / PixelsPerFullRotation;
    angles.y = glm::clamp(angles.y, -Pi / 2, Pi / 2);

    // Now, do the movement
    auto rotation = glm::mat3(glm::rotate(angles.x, glm::vec3(0, 1, 0)) * glm::rotate(angles.y, glm::vec3(1, 0, 0)));
    auto forward = rotation * glm::vec3(0, 0, -1);
    auto right = rotation * glm::vec3(1, 0, 0);

    if (window.getKey('W')) position += float(delta) * MoveSpeed * forward;
    if (window.getKey('S')) position -= float(delta) * MoveSpeed * forward;
    if (window.getKey('A')) position -= float(delta) * MoveSpeed * right;
    if (window.getKey('D')) position += float(delta) * MoveSpeed * right;

    lastPos = pos;
}

glm::mat4 Camera::getViewMatrix() const
{
    // The "camera" matrix is interpreted as being
    // translate(pos) * rotate(angles.x) * rotate(angles.y)
    // (read it right to left)
    // so we need to invert this in order to produce the inverse matrix
    return glm::rotate(-angles.y, glm::vec3(1, 0, 0))
        * glm::rotate(-angles.x, glm::vec3(0, 1, 0))
        * glm::translate(-position);
}
