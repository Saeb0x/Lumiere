#include "Window.h"
#include "utils/Logger.h"

using namespace Lumiere::Core;
using namespace Lumiere::Utils;

bool Window::Init(const std::string& title, unsigned int width, unsigned int height)
{
	if (!glfwInit())
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": glfwInit() Error\n");
		return false;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!m_Window)
	{
		Logger::Log(LogLevel::Fatal, __FUNCTION__, ": Window creation failed\n");
		glfwTerminate();
		return false;
	}
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Window initialized successfully\n");

	return true;
}

void Window::Loop()
{
	while (!glfwWindowShouldClose(m_Window))
	{
		glfwPollEvents();
	}
}

void Window::Cleanup()
{
	Logger::Log(LogLevel::Info, __FUNCTION__, ": Terminating Window\n");
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

