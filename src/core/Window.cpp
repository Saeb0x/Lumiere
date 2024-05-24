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
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": glfwInit() Error\n");
		return false;
	}

	#ifdef USE_VULKAN
	m_Title.append("-Vulkan");
	if (!glfwVulkanSupported())
	{
		glfwTerminate();
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Vulkan is not supported\n");
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Vulkan needs no context
	#else
	m_Title.append("-OpenGL");
	#endif

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(width, height, m_Title.c_str(), nullptr, nullptr);
	if (!m_Window)
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Window creation failed\n");
		glfwTerminate();
		return false;
	}

	#ifdef USE_VULKAN
	if (!InitVulkan())
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Vulkan can't be initialized\n");
		glfwTerminate();
		return false;
	}
	#else
	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);
	#endif 

	Logger::Log(LogLevel::Info, __FUNCTION__, ": Window initialized successfully\n");
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
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Terminating Window\n");

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
		Logger::Log(LogLevel::Fatal, __FUNCTION__, " error: no Vulkan extensions found, need at least 'VK_KHR_surface'\n");
		return false;
	}

	Logger::Log(LogLevel::Info, __FUNCTION__,": Found ", extensionCount , " Vulkan extensions\n");
	for (int i = 0; i < extensionCount; ++i) {
		Logger::Log(LogLevel::Info,__FUNCTION__, ": ", std::string(extensions[i]).c_str(), "\n");
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
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Could not create Vulkan instance(", result, ")\n");
		return false;
	}

	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);

	if (physicalDeviceCount == 0) {
		Logger::Log(LogLevel::Fatal, __FUNCTION__,": No Vulkan capable GPU found\n");
		return false;
	}

	std::vector<VkPhysicalDevice> devices;
	devices.resize(physicalDeviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, devices.data());

	Logger::Log(LogLevel::Info, __FUNCTION__, ": Found ", physicalDeviceCount, " physical device(s)\n");

	result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
	if (result != VK_SUCCESS) {
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Could not create Vulkan surface\n");
		return false;
	}

	return true;
}
#endif