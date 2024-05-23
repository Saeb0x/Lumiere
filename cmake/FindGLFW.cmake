## A find module script to locate GLFW library for Windows builds. GLFW is required to be stored in a fixed location on the system. 

set(GLFW_HEADER_SEARCH_DIRS
	"${CMAKE_SOURCE_DIR}/includes"
	"C:/Vendor/GLFW/include"
)

set(GLFW_LIB_SEARCH_DIRS
	"${CMAKE_SOURCE_DIR}/libs"
	"C:/Vendor/GLFW/lib"
)

# Check environment for root search directory
set(GLFW_ENV_ROOT $ENV{GLFW_ROOT})
if(NOT GLFW_ROOT AND GLFW_ENV_ROOT)
	set(GLFW_ROOT ${GLFW_ENV_ROOT})
endif()

# Put user specified location at beginning of search
if(GLFW_ROOT)
	list(INSERT GLFW_HEADER_SEARCH_DIRS 0 "${GLFW_ROOT}/include" )
	list(INSERT GLFW_LIB_SEARCH_DIRS 0 "${GLFW_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(GLFW_INCLUDE_DIR "GLFW/glfw3.h" PATHS ${GLFW_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(GLFW_LIBRARY NAMES glfw3 glfw PATHS ${GLFW_LIB_SEARCH_DIRS} )
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(glfw3 DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_DIR)