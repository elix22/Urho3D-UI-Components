# Set project name
project (Urho3D-UI-Components)

# Set minimum version
cmake_minimum_required (VERSION 2.8.6)



if (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
	if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
		# INTERFACE_LINK_LIBRARIES defines the link interface
		cmake_policy (SET CMP0022 NEW)
	endif ()
	if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
		# Disallow use of the LOCATION target property - therefore we set to OLD as we still need it
		cmake_policy (SET CMP0026 OLD)
		# MACOSX_RPATH is enabled by default
		cmake_policy (SET CMP0042 NEW)
	endif ()
endif ()

set ( NAME "Urho3D-UI-Components")

#macros
#include(${PROJECT_SOURCE_DIR}/macros.cmake)

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# Include Urho3D Cmake common module
include (UrhoCommon)

# Find Urho3D library
find_package (Urho3D REQUIRED)
include_directories (${PROJECT_SOURCE_DIR}/Source/Urho3D/UI ${URHO3D_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/Source/Samples/61_UITest/ ${PROJECT_SOURCE_DIR}/Source/Samples/61_UITest/NodeGraph/)

# Define target name
set (TARGET_NAME ${NAME})

# Define source files
#define_source_files ()

#sources
file(GLOB_RECURSE SOURCE_FILES 
	Source/Samples/61_UITest/*.cpp
	Source/Samples/61_UITest/*.h
	Source/Samples/61_UITest/*.inl
	Source/Samples/61_UITest/NodeGraph/*.h
	Source/Samples/61_UITest/NodeGraph/*.cpp
        Source//Urho3D/UI/*.h
)

#C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")

# Resrouces
set_output_directories (${PROJECT_SOURCE_DIR}/${PLATFORM_PREFIX}bin RUNTIME PDB)

# Setup target with resource copying
setup_main_executable ()






