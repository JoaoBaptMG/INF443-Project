#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mutex>
#include <vector>
#include <algorithm>
#include <functional>
#include <string_view>
#include <shared_mutex>
#include "Utils.hpp"
#include "Exception.hpp"
#include "util/array_view.hpp"

namespace glfw
{
	enum class MonitorEvent { Connected, Disconnected };

	using VideoMode = GLFWvidmode;
	using GammaRamp = GLFWgammaramp;

	class alignas(GLFWmonitor*) Monitor
	{
		static void prepareMonitors()
		{
			glfwSetMonitorCallback([](GLFWmonitor* monitor, int event)
				{
					// call the monitor callback
					std::unique_lock lock(monitorCallbackMutex);
					if (monitorCallback) monitorCallback(Monitor(monitor),
						event == GLFW_CONNECTED ? MonitorEvent::Connected : MonitorEvent::Disconnected);
				});
		}

		static inline std::mutex monitorCallbackMutex;
		static inline std::function<void(Monitor, MonitorEvent)> monitorCallback;

		GLFWmonitor* monitor;

		// Prevent initialization from the outside
		Monitor(GLFWmonitor* handle = nullptr) : monitor(handle) {}

	public:
		static auto getMonitors()
		{ 
			int cnt;
			auto monitorPtr = checkError(glfwGetMonitors(&cnt));

			// glfw::Monitor is pointer-interconvertible with GLFWmonitor*
			return util::array_view(reinterpret_cast<Monitor*>(monitorPtr), cnt);
		}

		static Monitor getPrimaryMonitor() { return Monitor(glfwGetPrimaryMonitor()); }

		// default copy constructors and operators
		Monitor(const Monitor&) = default;
		Monitor& operator=(const Monitor&) = default;

		// default move constructors and operators
		Monitor(Monitor&&) = default;
		Monitor& operator=(Monitor&&) = default;

		// API
		auto getVideoModes() const
		{
			int count;
			auto modes = checkError(glfwGetVideoModes(monitor, &count));
			return util::array_view(modes, count);
		}

		const VideoMode& getVideoMode() const { return *checkError(glfwGetVideoMode(monitor)); }

		auto getPhysicalSize() const
		{
			IntCoord coord;
			glfwGetMonitorPhysicalSize(monitor, &coord.x, &coord.y);
			checkError();
			return coord;
		}

		auto getContentScale() const
		{
			FloatCoord coord;
			glfwGetMonitorContentScale(monitor, &coord.x, &coord.y);
			checkError();
			return coord;
		}

		auto getPos() const
		{
			IntCoord coord;
			glfwGetMonitorPos(monitor, &coord.x, &coord.y);
			checkError();
			return coord;
		}

		auto getWorkArea() const
		{
			IntRectangle wa;
			glfwGetMonitorWorkarea(monitor, &wa.x, &wa.y, &wa.width, &wa.height);
			checkError();
			return wa;
		}

		auto getName() const { return std::string_view(checkError(glfwGetMonitorName(monitor))); }

		void setGammaRamp(const GammaRamp& ramp) { glfwSetGammaRamp(monitor, &ramp); checkError(); }
		const GammaRamp& getGammaRamp() const { return *checkError(glfwGetGammaRamp(monitor)); }

		void setGamma(float gamma) { glfwSetGamma(monitor, gamma); checkError(); }

		// Friends
		friend class Window;
	};
}
