#pragma once

#include <string>
#ifdef USE_VULKAN
#include <vulkan/vulkan.h>
#endif
#include <GLFW/glfw3.h>

namespace Lumiere 
{
	namespace Core
	{
		class Window
		{
		public:
			bool Init(const std::string& title, unsigned int width, unsigned int height);
			void Loop();
			void Cleanup();
		#ifdef USE_VULKAN
		private:
			bool InitVulkan();
		#endif
		private:
			GLFWwindow* m_Window = nullptr;
			std::string m_Title;
			#ifdef USE_VULKAN
			VkInstance m_Instance{};
			VkSurfaceKHR m_Surface{};
			#endif
		};
	}
}
