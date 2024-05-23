## A find module script to locate GLFW library for Windows builds. GLFW is required to be stored in a fixed location on the system. 

set(GLFW3_HEADER_SEARCH_DIRS
	"${CMAKE_SOURCE_DIR}/includes"
	"C:/Vendor/GLFW/include"
)

set(GLFW3_LIB_SEARCH_DIRS
	"${CMAKE_SOURCE_DIR}/libs"
	"C:/Vendor/GLFW/lib"
)

# Check environment for root search directory
set(GLFW3_ENV_ROOT $ENV{GLFW3_ROOT})
if(NOT GLFW3_ROOT AND GLFW3_ENV_ROOT)
	set(GLFW3_ROOT ${GLFW3_ENV_ROOT})
endif()

# Put user specified location at beginning of search
if(GLFW3_ROOT)
	list(INSERT GLFW3_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include" )
	list(INSERT GLFW3_LIB_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(GLFW3_INCLUDE_DIR "GLFW/glfw3.h" PATHS ${GLFW3_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(GLFW3_LIBRARY NAMES glfw3 glfw PATHS ${GLFW3_LIB_SEARCH_DIRS} )
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(glfw3 DEFAULT_MSG GLFW3_LIBRARY GLFW3_INCLUDE_DIR)