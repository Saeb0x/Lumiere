cmake_minimum_required(VERSION 3.19)

# C++17 as the minimum version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

if(UNIX)
	set(GLFW3_LIBRARY glfw)
endif()

include_directories(${GLFW3_INCLUDE_DIR})

if(MSVC)
	target_link_libraries(Lumiere ${GLFW3_LIBRARY})
else()
	# Clang and GCC may need explicit linking against the GNU Standard C++ 
	target_link_libraries(Lumiere ${GLFW3_LIBRARY} stdc++)
endif()
