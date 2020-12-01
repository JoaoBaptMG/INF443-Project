#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace glfw
{
	class Exception : public std::runtime_error
	{
	public:
		Exception(const char* what) : runtime_error(what) {}
	};

#define EXCEPTION_CLASS(name) class name final : public Exception \
{\
public:\
	name(const char* what) : Exception(what) {}\
}
	EXCEPTION_CLASS(NotInitialized);
	EXCEPTION_CLASS(NoCurrentContext);
	EXCEPTION_CLASS(InvalidEnum);
	EXCEPTION_CLASS(InvalidValue);
	EXCEPTION_CLASS(OutOfMemory);
	EXCEPTION_CLASS(ApiUnavailable);
	EXCEPTION_CLASS(VersionUnavailable);
	EXCEPTION_CLASS(PlatformError);
	EXCEPTION_CLASS(FormatUnavailable);
	EXCEPTION_CLASS(NoWindowContext);
#undef EXCEPTION_CLASS

	inline static void checkError(int code, const char* desc)
	{
		switch (code)
		{
		case GLFW_NOT_INITIALIZED: throw NotInitialized(desc);
		case GLFW_NO_CURRENT_CONTEXT: throw NoCurrentContext(desc);
		case GLFW_INVALID_ENUM: throw InvalidEnum(desc);
		case GLFW_INVALID_VALUE: throw InvalidValue(desc);
		case GLFW_OUT_OF_MEMORY: throw OutOfMemory(desc);
		case GLFW_API_UNAVAILABLE: throw ApiUnavailable(desc);
		case GLFW_VERSION_UNAVAILABLE: throw VersionUnavailable(desc);
		case GLFW_PLATFORM_ERROR: throw PlatformError(desc);
		case GLFW_FORMAT_UNAVAILABLE: throw FormatUnavailable(desc);
		case GLFW_NO_WINDOW_CONTEXT: throw NoWindowContext(desc);
		}
	}

	inline static void checkError()
	{
		const char* desc = nullptr;
		int error = glfwGetError(&desc);
		checkError(error, desc);
	}

	template <typename T>
	std::enable_if_t<!std::is_void_v<T>, T> checkError(T value)
	{
		checkError();
		return value;
	}
}