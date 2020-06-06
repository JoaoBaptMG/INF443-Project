#pragma once

#include "util/make_flags.hpp"
#include "Keys.hpp"

namespace glfw
{
    enum class ModKeys : int
    {
        Shift = 1,
        Ctrl = 2,
        Alt = 4,
        Super = 8,
        CapsLock = 16,
        NumLock = 32
    };

    MAKE_FLAGS(ModKeyFlags, ModKeys);

    enum MouseButton : int
    {
        Left, Right, Middle, Last = 7
    };

    enum class CursorMode : int
    {
        Normal = GLFW_CURSOR_NORMAL,
        Hidden = GLFW_CURSOR_HIDDEN,
        Disabled = GLFW_CURSOR_DISABLED
    };

    enum class Action : unsigned char // in order to be compatible with glfwGetJoystickButtons
    {
        Press = GLFW_PRESS,
        Release = GLFW_RELEASE,
        Repeat = GLFW_REPEAT
    };
}
