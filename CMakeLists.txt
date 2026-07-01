cmake_minimum_required(VERSION 3.27)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/stm32"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libraries"
)

include(ColoredOutput)
include(source_util)
include(ErrorUtils)

# A platform must be selected before any toolchain/defconfig path can be
# resolved.  Catch this early with a clear message rather than failing later
# with "toolchain file not found".
no_os_require(PLATFORM
    "PLATFORM is not set. Select a board via a CMake preset (cmake --preset <board>) or pass it explicitly, e.g. -DPLATFORM=maxim.")

# Check whether the build was started from the No-OS root or the library was included as a module
cmake_path(COMPARE ${CMAKE_SOURCE_DIR} EQUAL ${CMAKE_CURRENT_SOURCE_DIR} NATIVE_BUILD)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Auto-detect build context and set toolchain paths for unified presets
if(${NATIVE_BUILD})
    # In-tree build (full no-OS repo)
    message(STATUS "In-tree build detected")
    set(NO_OS_TOOLCHAIN_PATH "drivers/platform/${PLATFORM}/toolchain.cmake")
    set(NO_OS_DEFCONFIG_PATH "${CMAKE_SOURCE_DIR}/board_configs/${PLATFORM}/${BOARD_CONFIG_FILE}")

elseif(EXISTS "${CMAKE_SOURCE_DIR}/_deps/no-os-core-src")
    # Out-of-tree build with FetchContent
    message(STATUS "Out-of-tree build with FetchContent detected")
    set(NO_OS_TOOLCHAIN_PATH "_deps/no-os-core-src/drivers/platform/${PLATFORM}/toolchain.cmake")
    set(NO_OS_DEFCONFIG_PATH "${CMAKE_SOURCE_DIR}/_deps/no-os-core-src/board_configs/${PLATFORM}/${BOARD_CONFIG_FILE}")

elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/drivers")
    # Out-of-tree build using add_subdirectory (no-OS as subdirectory)
    message(STATUS "Out-of-tree build with no-OS as subdirectory detected")
    set(NO_OS_TOOLCHAIN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/drivers/platform/${PLATFORM}/toolchain.cmake")
    set(NO_OS_DEFCONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/board_configs/${PLATFORM}/${BOARD_CONFIG_FILE}")

elseif(DEFINED NO_OS_PATH AND EXISTS "${NO_OS_PATH}")
    # Out-of-tree build with local no-OS
    message(STATUS "Out-of-tree build with local no-OS: ${NO_OS_PATH}")
    set(NO_OS_TOOLCHAIN_PATH "${NO_OS_PATH}/drivers/platform/${PLATFORM}/toolchain.cmake")
    set(NO_OS_DEFCONFIG_PATH "${NO_OS_PATH}/board_configs/${PLATFORM}/${BOARD_CONFIG_FILE}")
else()
    message(FATAL_ERROR
        "Could not determine the no-OS build context. Run from the no-OS root, "
        "use FetchContent, add no-OS as a subdirectory, or set -DNO_OS_PATH=<path-to-no-OS>.")
endif()

# The toolchain path is derived from PLATFORM; a typo'd or unsupported platform
# produces a path that does not exist.  Fail here with the offending path
# instead of letting CMake report a generic "toolchain file not found".
if(NOT CMAKE_TOOLCHAIN_FILE)
    no_os_require_path("${CMAKE_SOURCE_DIR}/${NO_OS_TOOLCHAIN_PATH}"
        "Toolchain file not found: ${CMAKE_SOURCE_DIR}/${NO_OS_TOOLCHAIN_PATH}. Is PLATFORM=${PLATFORM} a supported platform?")
endif()

# Set DEFAULT_DEFCONFIG based on detected paths
if(DEFINED NO_OS_DEFCONFIG_PATH)
    set(DEFAULT_DEFCONFIG "${NO_OS_DEFCONFIG_PATH}")
endif()

# Handle PROJECT_DEFCONFIG based on build context
if(EXISTS "${CMAKE_SOURCE_DIR}/drivers")
    # In-tree build: Extract PROJECT_NAME from PROJECT_DEFCONFIG path
    if(DEFINED PROJECT_DEFCONFIG)
        get_filename_component(PROJECT_DEFCONFIG_DIR "${PROJECT_DEFCONFIG}" DIRECTORY)
        if(NOT "${PROJECT_DEFCONFIG_DIR}" STREQUAL "")
            set(NO_OS_PROJECT_NAME "${PROJECT_DEFCONFIG_DIR}")
            message(STATUS "Extracted PROJECT_NAME '${NO_OS_PROJECT_NAME}' from PROJECT_DEFCONFIG '${PROJECT_DEFCONFIG}'")
        endif()
    endif()
else()
    # Out-of-tree build: Check for project.conf or use override
    if(NOT DEFINED PROJECT_DEFCONFIG)
        if(EXISTS "${CMAKE_SOURCE_DIR}/project.conf")
            set(PROJECT_DEFCONFIG "project.conf")
            message(STATUS "Using default project.conf for out-of-tree build")
        endif()
    else()
        message(STATUS "Using custom PROJECT_DEFCONFIG: ${PROJECT_DEFCONFIG}")
    endif()
endif()

# Set toolchain file if not already set
if(DEFINED NO_OS_TOOLCHAIN_PATH AND NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/${NO_OS_TOOLCHAIN_PATH}")
endif()

# Default to a debuggable build when the user hasn't picked one.  The base
# compiler flags carry no -g; debug info only comes from the build-type
# specific flags (CMAKE_C_FLAGS_<TYPE>).  Without this, the ELF has no
# .debug_info/.debug_line, so the debugger shows no source.  Must be set
# before project() so the toolchain's build-type flags take effect.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
        "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
    message(STATUS "No CMAKE_BUILD_TYPE set, defaulting to RelWithDebInfo")
endif()

# Board compatibility checking
# include(BoardCompatibility)

project(no-os VERSION 1.0 LANGUAGES C CXX ASM)
include(FetchContent)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/CMakeCache.txt")
  set(IS_FRESH_CONFIGURE TRUE)
else()
  set(IS_FRESH_CONFIGURE FALSE)
endif()

message(STATUS "No-OS build")

find_package (Python3 COMPONENTS Interpreter Development)
# A Python interpreter is mandatory: it creates the build venv and runs the
# Kconfig config generator.  (Git is only used by the optional library cache,
# so it stays non-fatal.)
no_os_require(Python3_EXECUTABLE
    "Python 3 interpreter not found. Install Python 3 (python3) and ensure it is on PATH.")
find_package(Git)

if(${NATIVE_BUILD})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
  set(NO_OS_NATIVE_BUILD_MODE "--native")
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build PARENT_SCOPE)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib PARENT_SCOPE)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib PARENT_SCOPE)
  set(NO_OS_NATIVE_BUILD_MODE "")
endif()

set(NO_OS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT ${NATIVE_BUILD})
  set(NO_OS_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
endif()

set(CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/build/config)
set(KCONFIG_VARS_FILE ${CMAKE_CURRENT_BINARY_DIR}/build/config.cmake)
set(CONFIG_HEADER_FILE ${CMAKE_CURRENT_BINARY_DIR}/build/no_os_config.h)
set(CREATE_DOTCONFIG_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/build/create_dotconfig.cmake)

# Create build directory during configure phase
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build)

set(VENV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.no_os_venv")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  set(VENV_PYTHON_EXE "${VENV_DIR}/Scripts/python.exe")
else()
  set(VENV_PYTHON_EXE "${VENV_DIR}/bin/python3")
endif()

if(NOT EXISTS "${VENV_PYTHON_EXE}")
  message(STATUS "Creating Python virtual environment in ${VENV_DIR}")
  no_os_run_checked(
    WHAT "creating the Python virtual environment in ${VENV_DIR}"
    COMMAND ${Python3_EXECUTABLE} -m venv ${VENV_DIR})

  set(REQS_FILE "${NO_OS_DIR}/tools/scripts/requirements.txt")
  message(STATUS "Installing Python dependencies from ${REQS_FILE}")
  no_os_run_checked(
    WHAT "installing Python dependencies from ${REQS_FILE}"
    COMMAND ${VENV_PYTHON_EXE} -m pip install -r ${REQS_FILE})
endif()

if(NOT ${NATIVE_BUILD})
  set(VENV_PYTHON_EXE ${VENV_PYTHON_EXE} PARENT_SCOPE)
  # Variable already set in current scope above; PARENT_SCOPE propagates to including project
endif()

set(NO_OS_LIBRARY_TYPE "OBJECT" CACHE STRING "no-os library type (STATIC or OBJECT)")
set_property(CACHE NO_OS_LIBRARY_TYPE PROPERTY STRINGS STATIC OBJECT)
add_library(no-os ${NO_OS_LIBRARY_TYPE})

target_include_directories(
        no-os
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/build  # Generated no_os_config.h (takes precedence)
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Force-include the generated config header so source files don't need
# explicit #include "no_os_config.h" — matches the Make build where
# symbols arrive via -D CFLAGS.
target_compile_options(no-os PUBLIC -include no_os_config.h)
list(APPEND GENERATE_CONFIG_ARGS "--defconfig")
list(APPEND GENERATE_CONFIG_ARGS ${DEFAULT_DEFCONFIG})

# Add board-specific config if available (try variant-specific path first, flat fallback)
if(DEFINED NO_OS_PROJECT_NAME AND DEFINED BOARD)
  # Extract variant name from PROJECT_DEFCONFIG filename
  get_filename_component(VARIANT_NAME "${PROJECT_DEFCONFIG}" NAME_WE)

  if(${NATIVE_BUILD})
    set(_variant_cfg "${CMAKE_CURRENT_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/boards/${VARIANT_NAME}/${BOARD}.conf")
    set(_flat_cfg "${CMAKE_CURRENT_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/boards/${BOARD}.conf")
  else()
    set(_variant_cfg "${CMAKE_SOURCE_DIR}/boards/${VARIANT_NAME}/${BOARD}.conf")
    set(_flat_cfg "${CMAKE_SOURCE_DIR}/boards/${BOARD}.conf")
  endif()

  if(EXISTS "${_variant_cfg}")
    set(BOARD_DEFCONFIG_FILE "${_variant_cfg}")
  elseif(EXISTS "${_flat_cfg}")
    set(BOARD_DEFCONFIG_FILE "${_flat_cfg}")
  endif()

  if(DEFINED BOARD_DEFCONFIG_FILE AND EXISTS "${BOARD_DEFCONFIG_FILE}")
    message(STATUS "Adding board defconfig: ${BOARD_DEFCONFIG_FILE}")
    list(APPEND GENERATE_CONFIG_ARGS "${BOARD_DEFCONFIG_FILE}")
  endif()
endif()

if (DEFINED PROJECT_DEFCONFIG)
  message(STATUS "Adding project defconfig: ${PROJECT_DEFCONFIG}")
  if(${NATIVE_BUILD})
    set(PROJECT_DEFCONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/projects/${PROJECT_DEFCONFIG})
  else()
    set(PROJECT_DEFCONFIG_PATH ${CMAKE_SOURCE_DIR}/${PROJECT_DEFCONFIG})
  endif()
  list(APPEND GENERATE_CONFIG_ARGS ${PROJECT_DEFCONFIG_PATH})
endif()

target_compile_options(no-os PRIVATE -Wall)
target_link_options(no-os PRIVATE -Wl,--gc-sections)

# Collect all Kconfig files for dependency tracking
file(GLOB_RECURSE ALL_KCONFIG_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/Kconfig
  ${CMAKE_CURRENT_SOURCE_DIR}/*/Kconfig
  ${CMAKE_CURRENT_SOURCE_DIR}/*/*/Kconfig
  ${CMAKE_CURRENT_SOURCE_DIR}/*/*/*/Kconfig
  ${CMAKE_CURRENT_SOURCE_DIR}/*/*/*/*/Kconfig
)

set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${DEFAULT_DEFCONFIG}
  ${KCONFIG_VARS_FILE}
  ${CONFIG_FILE}
  ${ALL_KCONFIG_FILES}
)

if(IS_FRESH_CONFIGURE)
  file(REMOVE ${CONFIG_FILE})
endif()

# Generate initial config during configure if it doesn't exist
if(NOT EXISTS "${CONFIG_FILE}")
  no_os_run_checked(
    WHAT "generating the initial build config"
    COMMAND ${CMAKE_COMMAND} -E env
      KCONFIG_CONFIG=${CONFIG_FILE}
      srctree=${CMAKE_CURRENT_SOURCE_DIR}
      ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/scripts/generate_config.py
        ${GENERATE_CONFIG_ARGS}
        ${NO_OS_NATIVE_BUILD_MODE}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}/build
        --update
  )
else()
  no_os_run_checked(
    WHAT "refreshing the build config"
    COMMAND ${CMAKE_COMMAND} -E env
      KCONFIG_CONFIG=${CONFIG_FILE}
      srctree=${CMAKE_CURRENT_SOURCE_DIR}
      ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/scripts/generate_config.py
        ${NO_OS_NATIVE_BUILD_MODE}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}/build
  )
endif()

add_custom_command(
  OUTPUT ${KCONFIG_VARS_FILE} ${CONFIG_HEADER_FILE}

  COMMAND ${CMAKE_COMMAND} -E env
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    KCONFIG_CONFIG=${CONFIG_FILE}
    ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/scripts/generate_config.py
    ${GENERATE_CONFIG_ARGS}
    ${NO_OS_NATIVE_BUILD_MODE}
    --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
    --build_dir ${CMAKE_CURRENT_BINARY_DIR}/build
    --update

    DEPENDS ${CONFIG_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/scripts/generate_config.py ${ALL_KCONFIG_FILES} $<$<BOOL:${PROJECT_DEFCONFIG}>:${PROJECT_DEFCONFIG_PATH}>
    COMMENT "Generating CMake config variables and C header from config"
)

include(${KCONFIG_VARS_FILE})

if(NATIVE_BUILD)
    add_subdirectory(projects)
endif()
add_subdirectory(libraries)
add_subdirectory(network)
add_subdirectory(util)
add_subdirectory(iio)
add_subdirectory(drivers)

add_custom_target(KconfigVars ALL
    DEPENDS ${KCONFIG_VARS_FILE} ${CONFIG_HEADER_FILE}
)

add_custom_target(
  menuconfig
  COMMAND ${CMAKE_COMMAND} -E env
    KCONFIG_CONFIG=${CONFIG_FILE}
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    ${VENV_PYTHON_EXE} -m menuconfig
  COMMAND ${CMAKE_COMMAND} -E env
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/scripts/generate_config.py
    ${GENERATE_CONFIG_ARGS}
    ${NO_OS_NATIVE_BUILD_MODE}
    --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
    --build_dir ${CMAKE_CURRENT_BINARY_DIR}/build
  USES_TERMINAL
  VERBATIM
)

add_dependencies(no-os KconfigVars)

if(NATIVE_BUILD AND DEFINED NO_OS_PROJECT_NAME)
    message(STATUS "")
    message(STATUS "Build with: \ncmake --build ${CMAKE_BINARY_DIR} --target ${NO_OS_PROJECT_NAME}")
    message(STATUS "")
endif()
