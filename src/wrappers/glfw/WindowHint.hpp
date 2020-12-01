#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>
#include <string>

namespace glfw
{
	const struct DontCareTag {} DontCare;

	class WindowHint final
	{
		std::vector<std::pair<int, int>> hints;
		std::vector<std::pair<int, std::string>> stringHints;

		void applyHints() const
		{
			// Reset the hints
			glfwDefaultWindowHints();

			// Apply each one
			for (auto [hint, val] : hints) { glfwWindowHint(hint, val); }
			for (auto [hint, str] : stringHints) { glfwWindowHintString(hint, str.c_str()); }
		}

	public:
		// Standard constructors apply

		// Bool hints
		WindowHint& resizable(bool resizable = true) { hints.emplace_back(GLFW_RESIZABLE, resizable); return *this; }
		WindowHint& visible(bool visible = true) { hints.emplace_back(GLFW_VISIBLE, visible); return *this; }
		WindowHint& decorated(bool decorated = true) { hints.emplace_back(GLFW_DECORATED, decorated); return *this; }
		WindowHint& focused(bool focused = true) { hints.emplace_back(GLFW_FOCUSED, focused); return *this; }
		WindowHint& autoIconify(bool autoIconify = true) { hints.emplace_back(GLFW_AUTO_ICONIFY, autoIconify); return *this; }
		WindowHint& floating(bool floating = true) { hints.emplace_back(GLFW_FLOATING, floating); return *this; }
		WindowHint& maximized(bool maximized = true) { hints.emplace_back(GLFW_MAXIMIZED, maximized); return *this; }
		WindowHint& centerCursor(bool centerCursor = true) { hints.emplace_back(GLFW_CENTER_CURSOR, centerCursor); return *this; }
		WindowHint& transparentFramebuffer(bool transparentFramebuffer = true)
			{ hints.emplace_back(GLFW_TRANSPARENT_FRAMEBUFFER, transparentFramebuffer); return *this; }
		WindowHint& focusOnShow(bool focusOnShow = true) { hints.emplace_back(GLFW_FOCUS_ON_SHOW, focusOnShow); return *this; }
		WindowHint& scaleToMonitor(bool scaleToMonitor = true) { hints.emplace_back(GLFW_SCALE_TO_MONITOR, scaleToMonitor); return *this; }

		// Framebuffer hints
		WindowHint& redBits(int redBits) { hints.emplace_back(GLFW_RED_BITS, redBits); return *this; }
		WindowHint& redBits(DontCareTag = DontCare) { return redBits(GLFW_DONT_CARE); }
		WindowHint& greenBits(int greenBits) { hints.emplace_back(GLFW_GREEN_BITS, greenBits); return *this; }
		WindowHint& greenBits(DontCareTag = DontCare) { return greenBits(GLFW_DONT_CARE); }
		WindowHint& blueBits(int blueBits) { hints.emplace_back(GLFW_BLUE_BITS, blueBits); return *this; }
		WindowHint& blueBits(DontCareTag = DontCare) { return blueBits(GLFW_DONT_CARE); }
		WindowHint& alphaBits(int alphaBits) { hints.emplace_back(GLFW_ALPHA_BITS, alphaBits); return *this; }
		WindowHint& alphaBits(DontCareTag = DontCare) { return alphaBits(GLFW_DONT_CARE); }
		WindowHint& depthBits(int depthBits) { hints.emplace_back(GLFW_DEPTH_BITS, depthBits); return *this; }
		WindowHint& depthBits(DontCareTag = DontCare) { return depthBits(GLFW_DONT_CARE); }
		WindowHint& stencilBits(int stencilBits) { hints.emplace_back(GLFW_STENCIL_BITS, stencilBits); return *this; }
		WindowHint& stencilBits(DontCareTag = DontCare) { return stencilBits(GLFW_DONT_CARE); }
		WindowHint& stereo(bool stereo = true) { hints.emplace_back(GLFW_STEREO, stereo); return *this; }
		WindowHint& samples(int samples) { hints.emplace_back(GLFW_SAMPLES, samples); return *this; }
		WindowHint& samples(DontCareTag = DontCare) { return samples(GLFW_DONT_CARE); }
		WindowHint& srgbCapable(bool srgbCapable = true) { hints.emplace_back(GLFW_SRGB_CAPABLE, srgbCapable); return *this; }
		WindowHint& doublebuffer(bool doublebuffer = true) { hints.emplace_back(GLFW_DOUBLEBUFFER, doublebuffer); return *this; }
		WindowHint& refreshRate(int refreshRate) { hints.emplace_back(GLFW_REFRESH_RATE, refreshRate); return *this; }
		WindowHint& refreshRate(DontCareTag = DontCare) { return refreshRate(GLFW_DONT_CARE); }

		WindowHint& clientApi(ClientApi api) { hints.emplace_back(GLFW_CLIENT_API, static_cast<int>(api)); return *this; }
		WindowHint& contextCreationApi(ContextApi api) { hints.emplace_back(GLFW_CONTEXT_CREATION_API, static_cast<int>(api)); return *this; }
		WindowHint& contextVersion(int major, int minor, Profile profile = Profile::Any)
		{
			hints.emplace_back(GLFW_CONTEXT_VERSION_MAJOR, major);
			hints.emplace_back(GLFW_CONTEXT_VERSION_MINOR, minor);
			hints.emplace_back(GLFW_OPENGL_PROFILE, static_cast<int>(profile));
			return *this;
		}
		WindowHint& contextVersion(int major, int minor, ForwardCompatibleTag, Profile profile = Profile::Any)
		{
			hints.emplace_back(GLFW_CONTEXT_VERSION_MAJOR, major);
			hints.emplace_back(GLFW_CONTEXT_VERSION_MINOR, minor);
			hints.emplace_back(GLFW_OPENGL_PROFILE, static_cast<int>(profile));
			hints.emplace_back(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			return *this;
		}
		WindowHint& debugContext(bool debugContext = true) { hints.emplace_back(GLFW_OPENGL_DEBUG_CONTEXT, debugContext); return *this; }
		WindowHint& contextRobustness(Robustness robustness)
		{ 
			hints.emplace_back(GLFW_CONTEXT_ROBUSTNESS, static_cast<int>(robustness));
			return *this;
		}
		WindowHint& contextReleaseBehavior(ReleaseBehavior releaseBehavior)
		{
			hints.emplace_back(GLFW_CONTEXT_RELEASE_BEHAVIOR, static_cast<int>(releaseBehavior));
			return *this;
		}
		WindowHint& noError(bool noError = true) { hints.emplace_back(GLFW_NO_ERROR, noError); return *this; }

		WindowHint& cocoaRetinaFramebuffer(bool cocoaRetinaFramebuffer = true)
		{ 
			hints.emplace_back(GLFW_COCOA_RETINA_FRAMEBUFFER, cocoaRetinaFramebuffer);
			return *this;
		}
		WindowHint& cocoaFrameName(std::string frameName)
		{
			stringHints.emplace_back(GLFW_COCOA_FRAME_NAME, frameName);
			return *this;
		}
		WindowHint& cocoaGraphicsSwitching(bool cocoaGraphicsSwitching = true)
		{
			hints.emplace_back(GLFW_COCOA_GRAPHICS_SWITCHING, cocoaGraphicsSwitching);
			return *this;
		}

		WindowHint& x11ClassName(std::string className)
		{
			stringHints.emplace_back(GLFW_X11_CLASS_NAME, className);
			return *this;
		}
		WindowHint& x11InstanceName(std::string instanceName)
		{
			stringHints.emplace_back(GLFW_X11_INSTANCE_NAME, instanceName);
			return *this;
		}

		friend class Window;
	};
}
