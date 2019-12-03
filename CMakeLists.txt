# Copyright 2016 Analog Devices, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3, or (at your option)
#  any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with GNU Radio; see the file COPYING.  If not, write to
#  the Free Software Foundation, Inc., 51 Franklin Street,
#  Boston, MA 02110-1301, USA.

cmake_minimum_required(VERSION 3.1.3)
project(libm2k LANGUAGES CXX VERSION 0.1.0)

set(LIBM2K_VERSION_MAJOR 0)
set(LIBM2K_VERSION_MINOR 1)
set(LIBM2K_VERSION "${LIBM2K_VERSION_MAJOR}.${LIBM2K_VERSION_MINOR}")

project(libm2k LANGUAGES CXX VERSION ${LIBM2K_VERSION})

include(GNUInstallDirs)
include(FindGit OPTIONAL)

# build a shared library by default
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

# generate docs by default
option(ENABLE_DOC "Generate documentation with Doxygen" OFF)
option(BUILD_EXAMPLES "Build the default examples" ON)
option(ENABLE_LOG "Build with logging support" OFF)
option(ENABLE_EXCEPTIONS "Build with exception handling support" ON)
option(ENABLE_PYTHON "Build Python bindings" ON)
option(ENABLE_CSHARP "Build C# bindings" ON)
option(ENABLE_TOOLS "Build the tools" OFF)

if (ENABLE_DOC)
    add_subdirectory(doc)
endif()

if (ENABLE_PYTHON OR ENABLE_CSHARP)
	FIND_PACKAGE(SWIG REQUIRED)
endif()

# build with LIBM2K_EXPORTS defined in order to export everything that is marked with LIBM2K_API.
add_definitions(-DLIBM2K_EXPORTS=1)

set(CMAKE_SHARED_LIBRARY_PREFIX "")
if (WIN32)
	set(CMAKE_SHARED_MODULE_PREFIX "")
	set(CMAKE_STATIC_LIBRARY_PREFIX "")
	SET(CMAKE_IMPORT_LIBRARY_PREFIX "")
endif()

# Get the GIT hash of the latest commit
if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE LIBM2K_GIT_REPO
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (${LIBM2K_GIT_REPO} MATCHES ${CMAKE_CURRENT_SOURCE_DIR})
		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE LIBM2K_VERSION_GIT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	endif()
endif()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

set(LIBM2K_VERSION_FULL v${PROJECT_VERSION} - ${LIBM2K_VERSION_GIT})

if (ENABLE_LOG)
	add_definitions(-DLIBM2K_ENABLE_LOG)
	include(findLog4Cpp)
endif()

#Enable or disable exception handling
if (NOT ENABLE_EXCEPTIONS)
	remove_definitions("-fexceptions")
	add_definitions("-fno-exceptions")
	add_definitions(-D_EXCEPTIONS=0)
else()
	remove_definitions("-fno-exceptions")
	add_definitions("-fexceptions")
	add_definitions(-D_EXCEPTIONS=1)
endif()

configure_file(${CMAKE_SOURCE_DIR}/config.hpp.cmakein ${CMAKE_CURRENT_SOURCE_DIR}/include/libm2k/config.hpp @ONLY)

# Set the default install path to /usr
if (NOT WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "default install path" FORCE)
endif()

# get rid of Visual Studio's default "Debug" and "Release" output directories
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
endif()

set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
	CACHE PATH "Installation directory for pkgconfig (.pc) files")
mark_as_advanced(INSTALL_PKGCONFIG_DIR)

# handle RPATH issues on OS X
if(APPLE)
	set(CMAKE_MACOSX_RPATH ON)
	set(CMAKE_SKIP_BUILD_RPATH FALSE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_LIBDIR}" isSystemDir)
	if("${isSystemDir}" STREQUAL "-1")
		set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
	endif()
endif()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
endif()

list(FIND CMAKE_CXX_COMPILE_FEATURES cxx_constexpr OUT_CONSTEXPR)
if (NOT "${OUT_CONSTEXPR}" STREQUAL "-1")
	add_definitions(-DHAS_CONSTEXPR=1)
endif()

#Find libiio and link it to the targets generated by this project
find_library(IIO_LIBRARIES NAMES iio libiio)
find_path(IIO_INCLUDE_DIRS iio.h)

include_directories(
        ${CMAKE_SOURCE_DIR}
	${IIO_INCLUDE_DIRS}
)

#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -std=c++11")

#Add and build the source code subdirectory
add_subdirectory(src)

set(LIBM2K_PC ${CMAKE_CURRENT_BINARY_DIR}/libm2k.pc)
configure_file(libm2k.pc.cmakein ${LIBM2K_PC} @ONLY)
install(FILES ${LIBM2K_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")

configure_file(libm2k.iss.cmakein ${CMAKE_CURRENT_BINARY_DIR}/libm2k.iss @ONLY)

#Add and build python bindings
if (ENABLE_PYTHON AND SWIG_FOUND)
	message("---- Building Python bindings")
	add_subdirectory(bindings/python)
endif()

#Add and build C# bindings
if (ENABLE_CSHARP AND SWIG_FOUND)
	message("---- Building C# bindings")
	add_subdirectory(bindings/csharp)
endif()

#Add and build the examples
if (BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

#Add and build libm2k tools
if (ENABLE_TOOLS)
	add_subdirectory(tools)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	option(OSX_PACKAGE "Create a OSX package" ON)
	set(CMAKE_MACOSX_RPATH ON)
	set(SKIP_INSTALL_ALL ${OSX_PACKAGE})
endif()

if (NOT OSX_PACKAGE)
	# Support creating some basic binpkgs via `make package`.
	# Disabled if OSX_PACKAGE is enabled, as tarballs would end up empty otherwise.
	option(ENABLE_PACKAGING "Create .deb/.rpm or .tar.gz packages via 'make package'" OFF)

	if(ENABLE_PACKAGING)
#		if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#			include(cmake/DarwinPackaging.cmake)
#		endif()
		if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
			include(cmake/LinuxPackaging.cmake)
		endif()
	endif()
endif()
