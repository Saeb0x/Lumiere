#pragma once

#include <string>
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
		private:
			GLFWwindow* m_Window = nullptr;
		};
	}
}
