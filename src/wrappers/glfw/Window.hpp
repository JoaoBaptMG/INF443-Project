#pragma once

#include "Monitor.hpp"
#include "WindowHint.hpp"
#include "Utils.hpp"
#include "Exception.hpp"
#include "Input.hpp"
#include "Cursor.hpp"
#include "util/array_view.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shared_mutex>

namespace glfw
{
	class Window final
	{
		// Handle
		GLFWwindow* window;
		static inline Window* currentContext = nullptr;

	public:
		// default constructor
		Window() noexcept : window(nullptr) {}

		// Constructors
		Window(int width, int height, std::string title, Monitor monitor, const Window& share,
			const WindowHint& windowHint = WindowHint())
		{
			windowHint.applyHints();
			window = glfwCreateWindow(width, height, title.c_str(), monitor.monitor, share.window);
		}

		Window(int width, int height, std::string title, const WindowHint& windowHint = WindowHint())
			: Window(width, height, title, Monitor(), Window(), windowHint) {}
		Window(int width, int height, std::string title, Monitor monitor, const WindowHint& windowHint = WindowHint())
			: Window(width, height, title, monitor, Window(), windowHint) {}
		Window(int width, int height, std::string title, Window& share, const WindowHint& windowHint = WindowHint())
			: Window(width, height, title, Monitor(), share, windowHint) {}

		// borderless full screen
		static Window borderless(std::string title, const Monitor& monitor, const Window& share,
			WindowHint windowHint = WindowHint())
		{
			auto videoMode = monitor.getVideoMode();
			windowHint.redBits(videoMode.redBits);
			windowHint.greenBits(videoMode.greenBits);
			windowHint.blueBits(videoMode.blueBits);
			windowHint.refreshRate(videoMode.refreshRate);
			return Window(videoMode.width, videoMode.height, title, monitor, share, windowHint);
		}
		static Window borderless(std::string title, WindowHint windowHint = WindowHint())
		{
			return borderless(title, Monitor::getPrimaryMonitor(), Window(), windowHint);
		}
		static Window borderless(std::string title, const Monitor& monitor, WindowHint windowHint = WindowHint())
		{
			return borderless(title, monitor, Window(), windowHint);
		}
		static Window borderless(std::string title, const Window& share, WindowHint windowHint = WindowHint())
		{
			return borderless(title, Monitor::getPrimaryMonitor(), share, windowHint);
		}

		// Disable copying, enable moving
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		Window(Window&& other) noexcept : window(other.window) { other.window = 0; }
		Window& operator=(Window&& other) noexcept
		{
			std::swap(window, other.window);
			return *this;
		}

		// API
		bool shouldClose() const { return glfwWindowShouldClose(window); }
		void setShouldClose(bool close = true) { glfwSetWindowShouldClose(window, close);  }

		void setTitle(std::string title) { glfwSetWindowTitle(window, title.c_str());  }

		auto getPos() const
		{
			IntCoord coord;
			glfwGetWindowPos(window, &coord.x, &coord.y); 
			return coord;
		}
		void setPos(IntCoord coord) { setPos(coord.x, coord.y); }
		void setPos(int x, int y) { glfwSetWindowPos(window, x, y); }

		auto getSize() const
		{
			Size ws;
			glfwGetWindowPos(window, &ws.width, &ws.height);
			return ws;
		}
		void setSize(Size ws) { setPos(ws.width, ws.height); }
		void setSize(int width, int height) { glfwSetWindowPos(window, width, height); }

		void setSizeLimits(Size min, Size max)
		{
			setSizeLimits(min.width, min.height, max.width, max.height);
		}
		void setSizeLimits(int minwidth, int minheight, int maxwidth, int maxheight)
		{
			glfwSetWindowSizeLimits(window, minwidth, minheight, maxwidth, maxheight);
		}
		void setMaxSizeLimit(Size max) { setMaxSizeLimit(max.width, max.height); }
		void setMaxSizeLimit(int maxwidth, int maxheight)
		{
			glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, maxwidth, maxheight);
		}
		void setMinSizeLimit(Size min) { setMinSizeLimit(min.width, min.height); }
		void setMinSizeLimit(int minwidth, int minheight)
		{
			glfwSetWindowSizeLimits(window, minwidth, minheight, GLFW_DONT_CARE, GLFW_DONT_CARE);
		}
		void clearSizeLimits() { setSizeLimits(GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE); }

		void setAspectRatio(int numer, int denom) { glfwSetWindowAspectRatio(window, numer, denom); }
		void clearAspectRatio() { setAspectRatio(GLFW_DONT_CARE, GLFW_DONT_CARE); }

		auto getFramebufferSize() const
		{
			Size fs;
			glfwGetFramebufferSize(window, &fs.width, &fs.height);
			return fs;
		}

		auto getFrameSize() const
		{
			WindowFrameSize wfs;
			glfwGetWindowFrameSize(window, &wfs.left, &wfs.top, &wfs.right, &wfs.bottom);
			return wfs;
		}

		auto getContentScale() const
		{
			FloatCoord coord;
			glfwGetWindowContentScale(window, &coord.x, &coord.y);
			return coord;
		}

		auto getOpacity() const { return glfwGetWindowOpacity(window); }
		void setOpacity(float opacity) { glfwSetWindowOpacity(window, opacity); }

		void iconify() { glfwIconifyWindow(window); }
		void restore() { glfwRestoreWindow(window); }
		void maximize() { glfwMaximizeWindow(window); }
		void show() { glfwShowWindow(window); }
		void hide() { glfwHideWindow(window); }
		void focus() { glfwFocusWindow(window); }
		void requestAttention() { glfwRequestWindowAttention(window); }

		Monitor getMonitor() const { return Monitor(glfwGetWindowMonitor(window)); }

		void setMonitor(Monitor monitor, IntCoord position, Size size, int refreshRate)
		{
			setMonitor(monitor, position.x, position.y, size.width, size.height, refreshRate);
		}
		void setMonitor(Monitor monitor, IntRectangle rect, int refreshRate)
		{
			setMonitor(monitor, rect.x, rect.y, rect.width, rect.height, refreshRate);
		}
		void setMonitor(Monitor monitor, int x, int y, int width, int height, int refreshRate)
		{ 
			glfwSetWindowMonitor(window, monitor.monitor, x, y, width, height, refreshRate);
		}

		void setBorderlessFullscreen(Monitor& monitor = Monitor::getPrimaryMonitor())
		{
			auto videoMode = monitor.getVideoMode();
			glfwSetWindowMonitor(window, monitor.monitor, 0, 0, videoMode.width, videoMode.height, videoMode.refreshRate);
		}

		// Window attributes
		bool isFocused() const { return glfwGetWindowAttrib(window, GLFW_FOCUSED); }
		bool isIconified() const { return glfwGetWindowAttrib(window, GLFW_ICONIFIED); }
		bool isMaximized() const { return glfwGetWindowAttrib(window, GLFW_MAXIMIZED); }
		bool isBeingHovered() const { return glfwGetWindowAttrib(window, GLFW_HOVERED); }
		bool isVisible() const { return glfwGetWindowAttrib(window, GLFW_VISIBLE); }
		bool isResizable() const { return glfwGetWindowAttrib(window, GLFW_RESIZABLE); }
		bool isDecorated() const { return glfwGetWindowAttrib(window, GLFW_DECORATED); }
		bool isAutoIconify() const { return glfwGetWindowAttrib(window, GLFW_AUTO_ICONIFY); }
		bool isFloating() const { return glfwGetWindowAttrib(window, GLFW_FLOATING); }
		bool isTransparentFramebuffer() const { return glfwGetWindowAttrib(window, GLFW_TRANSPARENT_FRAMEBUFFER); }
		bool isFocusOnShow() const { return glfwGetWindowAttrib(window, GLFW_FOCUS_ON_SHOW); }

		// Setting attributes
		void setResizable(bool resizable) { glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable); }
		void setDecorated(bool decorated) { glfwSetWindowAttrib(window, GLFW_DECORATED, decorated); }
		void setAutoIconify(bool autoIconify) { glfwSetWindowAttrib(window, GLFW_AUTO_ICONIFY, autoIconify); }
		void setFloating(bool floating) { glfwSetWindowAttrib(window, GLFW_FLOATING, floating); }
		void setTransparentFramebuffer(bool transparentFramebuffer) 
		{ 
			glfwSetWindowAttrib(window, GLFW_TRANSPARENT_FRAMEBUFFER, transparentFramebuffer); 
		}
		void setFocusOnShow(bool focusOnShow) { glfwSetWindowAttrib(window, GLFW_FOCUS_ON_SHOW, focusOnShow); }

		// Context
		ClientApi getClientApi() const { return static_cast<ClientApi>(glfwGetWindowAttrib(window, GLFW_CLIENT_API)); }
		ContextApi getContextCreationApi() const
		{
			return static_cast<ContextApi>(glfwGetWindowAttrib(window, GLFW_CONTEXT_CREATION_API));
		}

		OpenGLVersion getContextVersion()
		{
			return
			{
				glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR),
				glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR),
				glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION)
			};
		}

		bool isContextForwardCompatible() const { return glfwGetWindowAttrib(window, GLFW_OPENGL_FORWARD_COMPAT); }
		bool isDebugContext() const { return glfwGetWindowAttrib(window, GLFW_OPENGL_FORWARD_COMPAT); }

		Profile getOpenGLProfile() const { return static_cast<Profile>(glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE)); }
		Robustness getContextRobustness() const
		{ 
			return static_cast<Robustness>(glfwGetWindowAttrib(window, GLFW_CONTEXT_ROBUSTNESS));
		}

		// Swap buffers
		void swapBuffers() { glfwSwapBuffers(window); }
		static void setSwapInterval(int interval) { glfwSwapInterval(interval); }

		// Context
		void makeCurrent()
		{
			glfwMakeContextCurrent(window);
			currentContext = this;
		}
		static void clearCurrent()
		{ 
			glfwMakeContextCurrent(nullptr); 
			currentContext = nullptr;
		}
		static Window& getCurrentContext() { return *currentContext; }

		// Input
		CursorMode getCursorMode() const { return static_cast<CursorMode>(glfwGetInputMode(window, GLFW_CURSOR)); }
		bool getStickyKeysEnabled() const { return glfwGetInputMode(window, GLFW_STICKY_KEYS); }
		bool getStickyMouseButtonsEnabled() const { return glfwGetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS); }
		bool getLockKeyModsEnabled() const { return glfwGetInputMode(window, GLFW_LOCK_KEY_MODS); }
		bool getRawMouseMotionEnabled() const { return glfwGetInputMode(window, GLFW_RAW_MOUSE_MOTION); }

		void setCursorMode(CursorMode mode) { glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode)); }
		void setStickyKeysEnabled(bool enable = true) { glfwSetInputMode(window, GLFW_STICKY_KEYS, enable); }
		void setStickyMouseButtonsEnabled(bool enable = true) { glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, enable); }
		void setLockKeyModsEnabled(bool enable = true) { glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, enable); }
		void setRawMouseMotionEnabled(bool enable = true) { glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, enable); }

		bool getKey(int key) const { return glfwGetKey(window, key); }
		bool getMouseButton(int button) const { return glfwGetMouseButton(window, button); }

		auto getCursorPos() const
		{
			DoubleCoord position;
			glfwGetCursorPos(window, &position.x, &position.y);
			return position;
		}

		void setCursorPos(double xpos, double ypos) { glfwSetCursorPos(window, xpos, ypos); }
		void setCursorPos(DoubleCoord position) { setCursorPos(position.x, position.y); }

		void setCursor(Cursor& cursor) { glfwSetCursor(window, cursor.cursor); }

		// Window event processing
	private:
		struct WindowCallbacks final
		{
			Window& window;
			std::function<void(Window&, int, int)> position;
			std::function<void(Window&, int, int)> size;
			std::function<void(Window&)> close;
			std::function<void(Window&)> refresh;
			std::function<void(Window&, bool)> focus;
			std::function<void(Window&, bool)> iconify;
			std::function<void(Window&, bool)> maximize;
			std::function<void(Window&, int, int)> bufferSize;
			std::function<void(Window&, float, float)> contentScale;

			WindowCallbacks(Window& window) : window(window) {}
		};

		static inline std::shared_mutex windowCallbackMutex;
		static inline std::unordered_map<GLFWwindow*, WindowCallbacks> windowCallbacks;

		template <typename Callback>
		void addWindowCallback(Callback callback)
		{
			std::unique_lock lock(windowCallbackMutex);
			auto it = windowCallbacks.find(window);
			if (it == windowCallbacks.end())
				it = windowCallbacks.emplace(window, WindowCallbacks(*this)).first;
			callback(it->second);
		}

		template <typename Callback>
		static void findWindowCallback(GLFWwindow* window, Callback callback)
		{
			std::shared_lock lock(windowCallbackMutex);
			auto it = windowCallbacks.find(window);
			if (it != windowCallbacks.end()) callback(it->second);
		}

	public:
		template <typename C>
		void setPosCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, int, int>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.position = callback; });
			glfwSetWindowPosCallback(window, [](GLFWwindow* window, int xpos, int ypos)
				{ findWindowCallback(window, [xpos, ypos](WindowCallbacks& cbs)
					{ cbs.position(cbs.window, xpos, ypos); }); });
		}

		template <typename C>
		void setSizeCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, int, int>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.size = callback; });
			glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
				{ findWindowCallback(window, [width, height](WindowCallbacks& cbs)
					{ cbs.size(cbs.window, width, height); }); });
		}

		template <typename C>
		void setCloseCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.close = callback; });
			glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
				{ findWindowCallback(window, [](WindowCallbacks& cbs) { cbs.close(cbs.window); }); });
		}

		template <typename C>
		void setRefreshCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.refresh = callback; });
			glfwSetWindowRefreshCallback(window, [](GLFWwindow* window)
				{ findWindowCallback(window, [](WindowCallbacks& cbs) { cbs.refresh(cbs.window); }); });
		}

		template <typename C>
		void setFocusCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, bool>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.focus = callback; });
			glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused)
				{ findWindowCallback(window, [focused](WindowCallbacks& cbs) { cbs.focus(cbs.window, focused); }); });
		}

		template <typename C>
		void setIconifyCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, bool>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.iconify = callback; });
			glfwSetWindowIconifyCallback(window, [](GLFWwindow* window, int iconified)
				{ findWindowCallback(window, [iconified](WindowCallbacks& cbs) { cbs.iconify(cbs.window, iconified); }); });
		}

		template <typename C>
		void setMaximizeCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, bool>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.maximize = callback; });
			glfwSetWindowMaximizeCallback(window, [](GLFWwindow* window, int maximized)
				{ findWindowCallback(window, [maximized](WindowCallbacks& cbs) { cbs.maximize(cbs.window, maximized); }); });
		}

		template <typename C>
		void setFramebufferSizeCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, int, int>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.bufferSize = callback; });
			glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
				{ findWindowCallback(window, [width, height](WindowCallbacks& cbs)
					{ cbs.bufferSize(cbs.window, width, height); }); });
		}

		template <typename C>
		void setContentScaleCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, float, float>);
			addWindowCallback([&callback](WindowCallbacks& cbs) { cbs.contentScale = callback; });
			glfwSetWindowContentScaleCallback(window, [](GLFWwindow* window, float xscale, float yscale)
				{ findWindowCallback(window, [xscale, yscale](WindowCallbacks& cbs)
					{ cbs.contentScale(cbs.window, xscale, yscale); }); });
		}

		// Input event processing
	private:
		struct InputCallbacks final
		{
			Window& window;
			std::function<void(Window&, glfw::MouseButton, glfw::Action, glfw::ModKeyFlags)> mouseButton;
			std::function<void(Window&, double, double)> cursorPos;
			std::function<void(Window&, bool)> cursorEnter;
			std::function<void(Window&, double, double)> scroll;
			std::function<void(Window&, int, int, glfw::Action, glfw::ModKeyFlags)> key;
			std::function<void(Window&, unsigned int)> charAction;
			std::function<void(Window&, util::array_view<const char*>)> drop;

			InputCallbacks(Window& window) : window(window) {}
		};

		static inline std::shared_mutex inputCallbackMutex;
		static inline std::unordered_map<GLFWwindow*, InputCallbacks> inputCallbacks;

		template <typename Callback>
		void addInputCallback(Callback callback)
		{
			std::unique_lock lock(inputCallbackMutex);
			auto it = inputCallbacks.find(window);
			if (it == inputCallbacks.end())
				it = inputCallbacks.emplace(window, WindowCallbacks(*this)).first;
			callback(it->second);
		}

		template <typename Callback>
		static void findInputCallback(GLFWwindow* window, Callback callback)
		{
			std::shared_lock lock(inputCallbackMutex);
			auto it = inputCallbacks.find(window);
			if (it != inputCallbacks.end()) callback(it->second);
		}

	public:
		template <typename C>
		void setMouseButtonCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, glfw::MouseButton, glfw::Action, glfw::ModKeyFlags>);
			addInputCallback([&callback](InputCallbacks& cbs) { cbs.mouseButton = callback; });
			glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
				{ findInputCallback(window, [button, action, mods](InputCallbacks& cbs)
					{ cbs.mouseButton(cbs.window, button, static_cast<glfw::Action>(action), glfw::ModKeyFlags(mods)); }); });
		}

		template <typename C>
		void setCursorPosCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, double, double>);
			addInputCallback([&callback](InputCallbacks& cbs) { cbs.cursorPos = callback; });
			glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
				{ findInputCallback(window, [xpos, ypos](InputCallbacks& cbs)
					{ cbs.cursorPos(cbs.window, xpos, ypos); }); });
		}

		template <typename C>
		void setCursorEnterCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, bool>);
			addInputCallback([&callback](InputCallbacks& cbs) { cbs.cursorEnter = callback; });
			glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered)
				{ findInputCallback(window, [entered](InputCallbacks& cbs)
					{ cbs.cursorEnter(cbs.window, entered); }); });
		}

		template <typename C>
		void setScrollCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, double, double>);
			addInputCallback([&callback](InputCallbacks& cbs) { cbs.scroll = callback; });
			glfwSetScrollCallback(window, [](GLFWwindow* window, double xscroll, double yscroll)
				{ findInputCallback(window, [xscroll, yscroll](InputCallbacks& cbs)
					{ cbs.scroll(cbs.window, xscroll, yscroll); }); });
		}

		template <typename C>
		void setKeyCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, int, int, glfw::Action, glfw::ModKeyFlags>);
			addInputCallback([&callback](InputCallbacks& cbs) { cbs.key = callback; });
			glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
				{ findInputCallback(window, [key, scancode, action, mods](InputCallbacks& cbs)
					{ cbs.key(cbs.window, key, scancode, static_cast<glfw::Action>(action), glfw::ModKeyFlags(mods)); }); });
		}

		template <typename C>
		void setCharCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, unsigned int>);
			addInputCallback([&callback](InputCallbacks& cbs) { cbs.charAction = callback; });
			glfwSetKeyCallback(window, [](GLFWwindow* window, unsigned int codepoint)
				{ findInputCallback(window, [codepoint](InputCallbacks& cbs)
					{ cbs.charAction(cbs.window, codepoint); }); });
		}

		template <typename C>
		void setDropCallback(C callback)
		{
			static_assert(std::is_invocable_r_v<void, C, Window&, util::array_view<const char*>>);
			addInputCallback([&callback](InputCallbacks& cbs) { cbs.drop = callback; });
			glfwSetKeyCallback(window, [](GLFWwindow* window, int pathCount, const char* paths[])
				{ findInputCallback(window, [pathCount, paths](InputCallbacks& cbs)
					{ cbs.drop(cbs.window, util::array_view(paths, pathCount)); }); });

		}

		void setViewport() const
		{
			auto size = getFramebufferSize();
			glViewport(0, 0, size.width, size.height);
		}

		// Destructor
		~Window()
		{ 
			if (window)
			{
				{ std::unique_lock lock(windowCallbackMutex); windowCallbacks.erase(window); }
				glfwDestroyWindow(window);
			}
		}

		// Operator
		operator GLFWwindow* () const { return window; }
	};
}