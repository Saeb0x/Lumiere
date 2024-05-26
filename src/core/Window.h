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
			std::string m_Title = "";
			#ifdef USE_VULKAN
			VkInstance m_Instance{};
			VkSurfaceKHR m_Surface{};
			#endif
		private:
			void HandleWindowCloseEvents();
			void HandleWindowMoveEvents(int xPos, int yPos);
			void HandleWindowMaximizeEvents(int maximized);
			void HandleWindowIconifyEvents(int iconified);
			void HandleWindowResizeEvents(int width, int height);

			void HandleKeyEvents(int key, int scancode, int action, int mods);
			void HandleMouseButtonEvents(int button, int action, int mods);
			void HandleMouseEnterLeaveEvents(int entered);
			void HandleMousePosEvent(int xPos, int yPos);
		};
	}
}
