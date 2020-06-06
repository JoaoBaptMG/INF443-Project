#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace glfw
{
	template <typename T>
	struct Coord { T x, y; };
	using IntCoord = Coord<int>;
	using FloatCoord = Coord<float>;
	using DoubleCoord = Coord<double>;

	struct IntRectangle { int x, y, width, height; };
	struct Size { int width, height; };

	struct WindowFrameSize { int left, top, right, bottom; };

	enum class ClientApi { OpenGL = GLFW_OPENGL_API, OpenGLES = GLFW_OPENGL_ES_API, None = GLFW_NO_API };
	enum class ContextApi { Native = GLFW_NATIVE_CONTEXT_API, EGL = GLFW_EGL_CONTEXT_API, OsMesa = GLFW_OSMESA_CONTEXT_API };
	enum class Profile { Core = GLFW_OPENGL_CORE_PROFILE, Compatibility = GLFW_OPENGL_COMPAT_PROFILE, Any = GLFW_OPENGL_ANY_PROFILE };
	const struct ForwardCompatibleTag {} ForwardCompatible;
	enum class Robustness
	{
		NoResetNotification = GLFW_NO_RESET_NOTIFICATION,
		LoseContextOnReset = GLFW_LOSE_CONTEXT_ON_RESET,
		None = GLFW_NO_ROBUSTNESS
	};
	enum class ReleaseBehavior
	{
		Any = GLFW_ANY_RELEASE_BEHAVIOR,
		Flush = GLFW_RELEASE_BEHAVIOR_FLUSH,
		None = GLFW_RELEASE_BEHAVIOR_NONE
	};

	struct OpenGLVersion
	{ 
		int major, minor, revision;
		auto toString() const
		{
			return std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(revision);
		}
	};
}