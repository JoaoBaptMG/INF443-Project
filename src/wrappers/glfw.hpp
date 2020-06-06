#pragma once

#include <string_view>
#include "glfw/Utils.hpp"
#include "glfw/Window.hpp"
#include "glfw/Monitor.hpp"
#include "glfw/Input.hpp"
#include "glfw/Joystick.hpp"
#include "glfw/Exception.hpp"

namespace glfw
{
	class InitHint final
	{
		bool _joystickHatButtons = true;
		bool _cocoaChdirResources = true;
		bool _cocoaMenubar = true;

		void applyHints() const
		{
			glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, _joystickHatButtons);
			glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, _cocoaChdirResources);
			glfwInitHint(GLFW_COCOA_MENUBAR, _cocoaMenubar);
		}

	public:
		
		InitHint& joystickHatButtons(bool value = true) { _joystickHatButtons = value; return *this; }
		InitHint& cocoaChdirResources(bool value = true) { _cocoaChdirResources = value; return *this; }
		InitHint& cocoaMenubar(bool value = true) { _cocoaMenubar = value; return *this; }

		friend struct InitGuard;
	};

	struct InitGuard final
	{
		InitGuard(const InitHint& hint = InitHint())
		{
			glfwSetErrorCallback([](int code, const char* desc) { checkError(code, desc); });

			hint.applyHints();
			glfwInit();
		}

		~InitGuard() { glfwTerminate(); }
	};

	inline static void pollEvents() { glfwPollEvents(); }
	inline static void waitEvents() { glfwWaitEvents(); }
	inline static void waitEvents(double timeout) { glfwWaitEventsTimeout(timeout); }
	inline static void postEmptyEvent() { glfwPostEmptyEvent(); }

	inline static bool isRawMouseMotionSupported() { return glfwRawMouseMotionSupported(); }

	inline static void setClipboardString(const char* string) { glfwSetClipboardString(nullptr, string); }
	inline static auto getClipboardString() { return glfwGetClipboardString(nullptr); }
}