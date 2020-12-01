#pragma once

#include "Exception.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace glfw
{
    enum class CursorShape : int
    {
        Arrow = 0x36001, TextInput, Crosshair, Hand, HResize, VResize
    };

    class alignas(GLFWcursor*) Cursor
    {
        GLFWcursor* cursor;

    public:
        Cursor(const GLFWimage& image, int xhot, int yhot)
        {
            cursor = glfwCreateCursor(&image, xhot, yhot);
            checkError();
        }

        Cursor(CursorShape shape)
        {
            cursor = glfwCreateStandardCursor(static_cast<int>(shape));
            checkError();
        }

        // Disable copying, enable moving
        Cursor(const Cursor&) = delete;
        Cursor& operator=(const Cursor&) = delete;

        Cursor(Cursor&& other) noexcept : cursor(other.cursor) { other.cursor = nullptr; }
        Cursor& operator=(Cursor&& other) noexcept
        {
            std::swap(cursor, other.cursor);
            return *this;
        }

        // destructor
        ~Cursor() { if (cursor) glfwDestroyCursor(cursor); }

        friend class Window;
    };
}
