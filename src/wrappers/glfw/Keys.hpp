#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Exception.hpp"

namespace glfw
{
    namespace key
    {
        constexpr auto Unknown = GLFW_KEY_UNKNOWN;
        constexpr auto Space = GLFW_KEY_SPACE;
        constexpr auto Apostrophe = GLFW_KEY_APOSTROPHE;
        constexpr auto Comma = GLFW_KEY_COMMA;
        constexpr auto Minus = GLFW_KEY_MINUS;
        constexpr auto Period = GLFW_KEY_PERIOD;
        constexpr auto Slash = GLFW_KEY_SLASH;
        constexpr auto _0 = GLFW_KEY_0;
        constexpr auto _1 = GLFW_KEY_1;
        constexpr auto _2 = GLFW_KEY_2;
        constexpr auto _3 = GLFW_KEY_3;
        constexpr auto _4 = GLFW_KEY_4;
        constexpr auto _5 = GLFW_KEY_5;
        constexpr auto _6 = GLFW_KEY_6;
        constexpr auto _7 = GLFW_KEY_7;
        constexpr auto _8 = GLFW_KEY_8;
        constexpr auto _9 = GLFW_KEY_9;
        constexpr auto Semicolon = GLFW_KEY_SEMICOLON;
        constexpr auto Equal = GLFW_KEY_EQUAL;
        constexpr auto A = GLFW_KEY_A;
        constexpr auto B = GLFW_KEY_B;
        constexpr auto C = GLFW_KEY_C;
        constexpr auto D = GLFW_KEY_D;
        constexpr auto E = GLFW_KEY_E;
        constexpr auto F = GLFW_KEY_F;
        constexpr auto G = GLFW_KEY_G;
        constexpr auto H = GLFW_KEY_H;
        constexpr auto I = GLFW_KEY_I;
        constexpr auto J = GLFW_KEY_J;
        constexpr auto K = GLFW_KEY_K;
        constexpr auto L = GLFW_KEY_L;
        constexpr auto M = GLFW_KEY_M;
        constexpr auto N = GLFW_KEY_N;
        constexpr auto O = GLFW_KEY_O;
        constexpr auto P = GLFW_KEY_P;
        constexpr auto Q = GLFW_KEY_Q;
        constexpr auto R = GLFW_KEY_R;
        constexpr auto S = GLFW_KEY_S;
        constexpr auto T = GLFW_KEY_T;
        constexpr auto U = GLFW_KEY_U;
        constexpr auto V = GLFW_KEY_V;
        constexpr auto W = GLFW_KEY_W;
        constexpr auto X = GLFW_KEY_X;
        constexpr auto Y = GLFW_KEY_Y;
        constexpr auto Z = GLFW_KEY_Z;
        constexpr auto LeftBracket = GLFW_KEY_LEFT_BRACKET;
        constexpr auto Backslash = GLFW_KEY_BACKSLASH;
        constexpr auto RightBracket = GLFW_KEY_RIGHT_BRACKET;
        constexpr auto GraveAccent = GLFW_KEY_GRAVE_ACCENT;
        constexpr auto World1 = GLFW_KEY_WORLD_1;
        constexpr auto World2 = GLFW_KEY_WORLD_2;
        constexpr auto Escape = GLFW_KEY_ESCAPE;
        constexpr auto Enter = GLFW_KEY_ENTER;
        constexpr auto Tab = GLFW_KEY_TAB;
        constexpr auto Backspace = GLFW_KEY_BACKSPACE;
        constexpr auto Insert = GLFW_KEY_INSERT;
        constexpr auto Delete = GLFW_KEY_DELETE;
        constexpr auto Right = GLFW_KEY_RIGHT;
        constexpr auto Left = GLFW_KEY_LEFT;
        constexpr auto Down = GLFW_KEY_DOWN;
        constexpr auto Up = GLFW_KEY_UP;
        constexpr auto PageUp = GLFW_KEY_PAGE_UP;
        constexpr auto PageDown = GLFW_KEY_PAGE_DOWN;
        constexpr auto Home = GLFW_KEY_HOME;
        constexpr auto End = GLFW_KEY_END;
        constexpr auto CapsLock = GLFW_KEY_CAPS_LOCK;
        constexpr auto ScrollLock = GLFW_KEY_SCROLL_LOCK;
        constexpr auto NumLock = GLFW_KEY_NUM_LOCK;
        constexpr auto PrintScreen = GLFW_KEY_PRINT_SCREEN;
        constexpr auto Pause = GLFW_KEY_PAUSE;
        constexpr auto F1 = GLFW_KEY_F1;
        constexpr auto F2 = GLFW_KEY_F2;
        constexpr auto F3 = GLFW_KEY_F3;
        constexpr auto F4 = GLFW_KEY_F4;
        constexpr auto F5 = GLFW_KEY_F5;
        constexpr auto F6 = GLFW_KEY_F6;
        constexpr auto F7 = GLFW_KEY_F7;
        constexpr auto F8 = GLFW_KEY_F8;
        constexpr auto F9 = GLFW_KEY_F9;
        constexpr auto F10 = GLFW_KEY_F10;
        constexpr auto F11 = GLFW_KEY_F11;
        constexpr auto F12 = GLFW_KEY_F12;
        constexpr auto F13 = GLFW_KEY_F13;
        constexpr auto F14 = GLFW_KEY_F14;
        constexpr auto F15 = GLFW_KEY_F15;
        constexpr auto F16 = GLFW_KEY_F16;
        constexpr auto F17 = GLFW_KEY_F17;
        constexpr auto F18 = GLFW_KEY_F18;
        constexpr auto F19 = GLFW_KEY_F19;
        constexpr auto F20 = GLFW_KEY_F20;
        constexpr auto F21 = GLFW_KEY_F21;
        constexpr auto F22 = GLFW_KEY_F22;
        constexpr auto F23 = GLFW_KEY_F23;
        constexpr auto F24 = GLFW_KEY_F24;
        constexpr auto F25 = GLFW_KEY_F25;
        constexpr auto Kp0 = GLFW_KEY_KP_0;
        constexpr auto Kp1 = GLFW_KEY_KP_1;
        constexpr auto Kp2 = GLFW_KEY_KP_2;
        constexpr auto Kp3 = GLFW_KEY_KP_3;
        constexpr auto Kp4 = GLFW_KEY_KP_4;
        constexpr auto Kp5 = GLFW_KEY_KP_5;
        constexpr auto Kp6 = GLFW_KEY_KP_6;
        constexpr auto Kp7 = GLFW_KEY_KP_7;
        constexpr auto Kp8 = GLFW_KEY_KP_8;
        constexpr auto Kp9 = GLFW_KEY_KP_9;
        constexpr auto KpDecimal = GLFW_KEY_KP_DECIMAL;
        constexpr auto KpDivide = GLFW_KEY_KP_DIVIDE;
        constexpr auto KpMultiply = GLFW_KEY_KP_MULTIPLY;
        constexpr auto KpSubtract = GLFW_KEY_KP_SUBTRACT;
        constexpr auto KpAdd = GLFW_KEY_KP_ADD;
        constexpr auto KpEnter = GLFW_KEY_KP_ENTER;
        constexpr auto KpEqual = GLFW_KEY_KP_EQUAL;
        constexpr auto LeftShift = GLFW_KEY_LEFT_SHIFT;
        constexpr auto LeftControl = GLFW_KEY_LEFT_CONTROL;
        constexpr auto LeftAlt = GLFW_KEY_LEFT_ALT;
        constexpr auto LeftSuper = GLFW_KEY_LEFT_SUPER;
        constexpr auto RightShift = GLFW_KEY_RIGHT_SHIFT;
        constexpr auto RightControl = GLFW_KEY_RIGHT_CONTROL;
        constexpr auto RightAlt = GLFW_KEY_RIGHT_ALT;
        constexpr auto RightSuper = GLFW_KEY_RIGHT_SUPER;
        constexpr auto Menu = GLFW_KEY_MENU;
        constexpr auto Last = GLFW_KEY_LAST;

        inline static auto getName(int key, int code = 0) { return checkError(glfwGetKeyName(key, code)); }
        inline static auto getScancode(int key) { return checkError(glfwGetKeyScancode(key)); }
    };
}


