#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Input.hpp"
#include "Exception.hpp"
#include "util/array_view.hpp"
#include <shared_mutex>
#include <mutex>

namespace glfw
{
    enum class JoystickHat : unsigned char
    {
        Centered = 0, Up = 1, Right = 2, Down = 4, Left = 8,
        RightUp = 3, RightDown = 6, LeftUp = 9, LeftDown = 12
    };

    MAKE_FLAGS(JoystickHatFlags, JoystickHat);

    enum class JoystickEvent : int { Connected = GLFW_CONNECTED, Disconnected = GLFW_DISCONNECTED };

    enum class GamepadAxis : int
    {
        LeftX, LeftY, RightX, RightY, LeftTrigger, RightTrigger, Last = RightTrigger
    };

    enum class GamepadButton : int
    {
        A, B, X, Y, LeftBumper, RightBumper, Back, Start, Guide, LeftThumb, RightTumb,
        DpadUp, DpadRight, DpadDown, DpadLeft, Last = DpadLeft,
        Cross = A, Circle = B, Square = X, Triangle = Y
    };

    class Joystick final
    {
        int jid;

        static inline std::function<void(Joystick, JoystickEvent)> joystickCallback;
        static inline std::mutex joystickCallbackMutex;

    public:
        explicit Joystick(int jid) : jid(jid) {}

        bool isPresent() const { return glfwJoystickPresent(jid); }

        auto getAxes() const
        {
            int count;
            auto axes = checkError(glfwGetJoystickAxes(jid, &count));
            return util::array_view(axes, count);
        }

        auto getButtons() const
        {
            int count;
            auto buttons = checkError(glfwGetJoystickButtons(jid, &count));
            return util::array_view(reinterpret_cast<const Action*>(buttons), count);
        }

        auto getHats() const
        {
            int count;
            auto hats = checkError(glfwGetJoystickHats(jid, &count));
            return util::array_view(reinterpret_cast<const JoystickHatFlags*>(hats), count);
        }
        // JoystickHatFlags is pointer-interconvertible with unsigned char

        auto getName() const { return checkError(glfwGetJoystickName(jid)); }
        auto getGUID() const { return checkError(glfwGetJoystickGUID(jid)); }

        bool isGamepad() const { return checkError(glfwJoystickIsGamepad(jid)); }

        template <typename C>
        static void setCallback(C callback)
        {
            static_assert(std::is_invocable_r_v<void, C, Joystick, JoystickEvent>);

            {
                std::unique_lock lock(joystickCallbackMutex);
                joystickCallback = callback;
            }

            glfwSetJoystickCallback([](int jid, int event)
                {
                    std::unique_lock lock(joystickCallbackMutex);
                    joystickCallback(Joystick(jid), static_cast<JoystickEvent>(event));
                });
        }

        friend class Gamepad;
    };

    using GamepadState = GLFWgamepadstate;

    class Gamepad final
    {
        int jid;

    public:
        Gamepad(Joystick joystick) : jid(joystick.jid)
        {
            if (!joystick.isGamepad())
                throw InvalidValue("Current joystick is not a gamepad!");
        }

        Gamepad(int jid) : Gamepad(Joystick(jid)) {}

        auto getName() const { return checkError(glfwGetGamepadName(jid)); }

        auto getState() const
        {
            GamepadState state;
            glfwGetGamepadState(jid, &state);
            checkError();
            return state;
        }

        static void updateMappings(const char* string)
        {
            if (!checkError(glfwUpdateGamepadMappings(string)))
                throw PlatformError("An error occurred while trying to update the gamepad mappings!");
        }
    };
}