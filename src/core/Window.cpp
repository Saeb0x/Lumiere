#include "Window.h"
#include "utils/Logger.h"

#ifdef USE_VULKAN
#include <vector>
#endif

using namespace Lumiere::Core;
using namespace Lumiere::Utils;

bool Window::Init(const std::string& title, unsigned int width, unsigned int height)
{
	m_Title = title;

	if (!glfwInit())
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": glfwInit() Error");
		return false;
	}

	#ifdef USE_VULKAN
	m_Title.append("-Vulkan");
	if (!glfwVulkanSupported())
	{
		glfwTerminate();
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Vulkan is not supported");
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Vulkan needs no context
	#else
	m_Title.append("-OpenGL");
	#endif

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_Window = glfwCreateWindow(width, height, m_Title.c_str(), nullptr, nullptr);
	if (!m_Window)
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Window creation failed");
		glfwTerminate();
		return false;
	}

	glfwSetWindowUserPointer(m_Window, this);

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleWindowCloseEvents();
		}
	);
	glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos) 
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleWindowMoveEvents(xpos, ypos);
		}
	);
	glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleWindowMaximizeEvents(maximized);
		}
	);
	glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleWindowIconifyEvents(iconified);
		}
	);
	glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleKeyEvents(key, scancode, action, mods);
		}
	);
	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleMouseButtonEvents(button, action, mods);
		}
	);
	glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered)
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleMouseEnterLeaveEvents(entered);
		}
	);
	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleMousePosEvent(xpos, ypos);
		}
	);
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			auto thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
			thisWindow->HandleWindowResizeEvents(width, height);
		}
	);

	#ifdef USE_VULKAN
	if (!InitVulkan())
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Vulkan can't be initialized");
		glfwTerminate();
		return false;
	}

	#else
	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);
	#endif 

	Logger::Log(LogLevel::Info, __FUNCTION__, ": Window initialized successfully");
	return true;
}

void Window::Loop()
{
	float color = 0.0f;
	while (!glfwWindowShouldClose(m_Window))
	{
		#ifdef USE_VULKAN
		// Vulkan specific drawing code
		#else
		color >= 1.0f ? color = 0.0f : color += 0.01f;
		glClearColor(color, color, color, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(m_Window);
		#endif	
	
		glfwPollEvents();
	}
}

void Window::Cleanup()
{
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Terminating Window");

	#ifdef USE_VULKAN
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
	#endif

	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

#ifdef USE_VULKAN
bool Window::InitVulkan()
{
	VkResult result = VK_ERROR_UNKNOWN;

	VkApplicationInfo appInfo{};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = m_Title.c_str();
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
	appInfo.pEngineName = "Lumiere";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 1, 0);

	uint32_t extensionCount = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	if (extensionCount == 0) {
		Logger::Log(LogLevel::Fatal, __FUNCTION__, " error: no Vulkan extensions found, need at least 'VK_KHR_surface'");
		return false;
	}

	Logger::Log(LogLevel::Info, __FUNCTION__,": Found ", extensionCount , " Vulkan extensions");
	for (int i = 0; i < extensionCount; ++i) {
		Logger::Log(LogLevel::Info,__FUNCTION__, ": ", std::string(extensions[i]).c_str());
	}

	VkInstanceCreateInfo mCreateInfo{};
	mCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	mCreateInfo.pNext = nullptr;
	mCreateInfo.pApplicationInfo = &appInfo;
	mCreateInfo.enabledExtensionCount = extensionCount;
	mCreateInfo.ppEnabledExtensionNames = extensions;
	mCreateInfo.enabledLayerCount = 0;

	result = vkCreateInstance(&mCreateInfo, nullptr, &m_Instance);
	if (result != VK_SUCCESS) {
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Could not create Vulkan instance(", result, ")");
		return false;
	}

	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);

	if (physicalDeviceCount == 0) {
		Logger::Log(LogLevel::Fatal, __FUNCTION__,": No Vulkan capable GPU found");
		return false;
	}

	std::vector<VkPhysicalDevice> devices;
	devices.resize(physicalDeviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, devices.data());

	Logger::Log(LogLevel::Info, __FUNCTION__, ": Found ", physicalDeviceCount, " physical device(s)");

	result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
	if (result != VK_SUCCESS) {
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Could not create Vulkan surface");
		return false;
	}

	return true;
}
#endif

void Window::HandleWindowCloseEvents()
{
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Window got close event, bye");
}

void Window::HandleWindowMoveEvents(int xPos, int yPos)
{
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Window moved to (", xPos, ", ", yPos, ")");
}

void Window::HandleWindowMaximizeEvents(int maximized)
{
	if (maximized)
		Logger::Log(LogLevel::Info, __FUNCTION__, ": Window has been maximized");
	else
		Logger::Log(LogLevel::Info, __FUNCTION__, ": Window has been restored");
}

void Window::HandleWindowIconifyEvents(int iconified)
{
	if (iconified)
		Logger::Log(LogLevel::Info, __FUNCTION__, ": Window has been iconified");
	else
		Logger::Log(LogLevel::Info, __FUNCTION__, ": Window has been restored");
}

void Lumiere::Core::Window::HandleWindowResizeEvents(int width, int height)
{
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Window has been resized to (", width, ",", height, ")");
}

void Window::HandleKeyEvents(int key, int scancode, int action, int mods)
{
	std::string actionName;
	switch (action) 
	{
	case GLFW_PRESS:
		actionName = "pressed"; break;
	case GLFW_RELEASE:
		actionName = "released"; break;
	case GLFW_REPEAT:
		actionName = "repeated"; break;
	default:
		actionName = "invalid"; break;
	}

	const char* keyName = glfwGetKeyName(key, scancode);
	
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Key ", keyName, " (ASCII key code: ", key, ", Scancode (platform-specific): ", scancode, ") ", "is ", actionName);
}

void Window::HandleMouseButtonEvents(int button, int action, int mods)
{
	std::string actionName;
	switch (action) 
	{
	case GLFW_PRESS:
		actionName = "pressed"; break;
	case GLFW_RELEASE:
		actionName = "released"; break;
	case GLFW_REPEAT:
		actionName = "repeated"; break;
	default:
		actionName = "invalid"; break;
	}

	std::string buttonName;
	switch (button) 
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		buttonName = "left"; break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		buttonName = "right"; break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		buttonName = "middle"; break;
	default:
		buttonName = "other"; break;
	}

	Logger::Log(LogLevel::Info, __FUNCTION__, ": Mouse ", buttonName, " button (", button, ") ", actionName);
}

void Window::HandleMouseEnterLeaveEvents(int entered)
{
	if (entered)
		Logger::Log(LogLevel::Info, __FUNCTION__, ": Mouse entered window");
	else
		Logger::Log(LogLevel::Info, __FUNCTION__, ": Mouse left window");
}

void Window::HandleMousePosEvent(int xPos, int yPos)
{
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Mouse position is (", xPos, ", ", yPos, ")");
}
