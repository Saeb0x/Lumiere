cmake_minimum_required(VERSION 3.19)

# C++17 as the minimum version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(USE_VULKAN "Use Vulkan for rendering instead of OpenGL, which is enabled by default" OFF)

# Append custom find modules for Windows builds
if(WIN32)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

project(Lumiere)

file(GLOB SOURCES
	src/*.cpp
	src/core/utils/*.cpp
	src/core/*.cpp
)

add_executable(Lumiere ${SOURCES})

find_package(glfw3 3.3 REQUIRED)
if(USE_VULKAN)
	find_package(Vulkan REQUIRED)
	set(RENDERER_API Vulkan::Vulkan)
	add_compile_definitions(USE_VULKAN)
else()
	# GL Vendor-Neutral Dispatch
	set(OpenGL_GL_PREFERENCE GLVND)
	find_package(OpenGL REQUIRED)
	set(RENDERER_API OpenGL::GL)
endif()

if(UNIX)
	set(GLFW3_LIBRARY glfw)
endif()

include_directories(${GLFW3_INCLUDE_DIR})

if(MSVC)
	target_link_libraries(Lumiere ${GLFW3_LIBRARY} ${RENDERER_API})
else()
	# Clang and GCC may need explicit linking against the GNU Standard C++ 
	target_link_libraries(Lumiere ${GLFW3_LIBRARY} ${RENDERER_API} stdc++)
endif()
