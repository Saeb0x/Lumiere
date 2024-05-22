cmake_minimum_required(VERSION 3.19)

# C++17 as the minimum version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("Lumiere")

file(GLOB SOURCES
	src/*.cpp
	src/core/utils/*.cpp
)

add_executable(Lumiere ${SOURCES})
