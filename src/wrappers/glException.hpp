#pragma once

#include "glad/glad.h"
#include <stdexcept>

namespace gl
{
#define EXCEPTION_CLASS(name) class name final : public std::runtime_error \
{\
public:\
	name() : runtime_error("OpenGL Error") {}\
}

	EXCEPTION_CLASS(InvalidEnum);
	EXCEPTION_CLASS(InvalidValue);
	EXCEPTION_CLASS(InvalidOperation);
	EXCEPTION_CLASS(InvalidFramebufferOperation);
	EXCEPTION_CLASS(OutOfMemory);
	EXCEPTION_CLASS(StackUnferflow);
	EXCEPTION_CLASS(StackOverflow);
#undef EXCEPTION_CLASS

	inline static void checkError()
	{
		switch (glGetError())
		{
		case GL_INVALID_ENUM: throw InvalidEnum();
		case GL_INVALID_VALUE: throw InvalidValue();
		case GL_INVALID_OPERATION: throw InvalidOperation();
		case GL_INVALID_FRAMEBUFFER_OPERATION: throw InvalidFramebufferOperation();
		case GL_OUT_OF_MEMORY: throw OutOfMemory();
		case GL_STACK_UNDERFLOW: throw StackUnferflow();
		case GL_STACK_OVERFLOW: throw StackOverflow();
		}
	}

	template <typename T>
	std::enable_if_t<!std::is_void_v<T>, T> checkError(T value)
	{
		checkError();
		return value;
	}
}
