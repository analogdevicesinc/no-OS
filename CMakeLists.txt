cmake_minimum_required(VERSION 3.27)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Enable colored output globally
include(ColoredOutput)

# Auto-detect build context and set toolchain paths for unified presets
if(EXISTS "${CMAKE_SOURCE_DIR}/drivers")
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

# Board compatibility checking
include(BoardCompatibility)

project(no-os VERSION 1.0 LANGUAGES C CXX ASM)
include(FetchContent)


# Check whether the build was started from the No-OS root or the library was included as a module
cmake_path(COMPARE ${CMAKE_SOURCE_DIR} EQUAL ${CMAKE_CURRENT_SOURCE_DIR} NATIVE_BUILD)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/CMakeCache.txt")
  set(IS_FRESH_CONFIGURE TRUE)
else()
  set(IS_FRESH_CONFIGURE FALSE)
endif()

message(STATUS "No-OS build")

find_package (Python3 COMPONENTS Interpreter Development)
find_package(Git)

function(no_os_sources_ifdef feature_toggle source)
  if(${feature_toggle})
    message(STATUS "Adding ${source}")
    target_sources(no-os PRIVATE ${source} ${ARGN})
  endif()
endfunction()

function(no_os_include_dir_ifdef feature_toggle include_dir)
  if(${feature_toggle})
    message(STATUS "Adding ${include_dir}")
    target_include_directories(no-os PUBLIC ${include_dir})
  endif()
endfunction()

# Board specific defconfig
list(APPEND CONFIG_FILES ${DEFAULT_DEFCONFIG})

message(STATUS "Project defconfig: ${PROJECT_DEFCONFIG}")

set(NO_OS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT ${NATIVE_BUILD})
  set(NO_OS_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
endif()

set(CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/.config)
set(KCONFIG_VARS_FILE ${CMAKE_CURRENT_BINARY_DIR}/config.cmake)
set(CREATE_DOTCONFIG_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/create_dotconfig.cmake)

set(VENV_DIR "${CMAKE_CURRENT_BINARY_DIR}/.no_os_venv")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  set(VENV_PYTHON_EXE "${VENV_DIR}/Scripts/python.exe")
else()
  set(VENV_PYTHON_EXE "${VENV_DIR}/bin/python3")
endif()

if(NOT EXISTS "${VENV_PYTHON_EXE}")
  message(STATUS "Creating Python virtual environment in ${VENV_DIR}")
  execute_process(COMMAND ${Python3_EXECUTABLE} -m venv ${VENV_DIR})

  set(REQS_FILE "${NO_OS_DIR}/tools/scripts/requirements.txt")
  message(STATUS "Installing Python dependencies from ${REQS_FILE}")
  execute_process(COMMAND ${VENV_PYTHON_EXE} -m pip install -r ${REQS_FILE})
endif()

if(NOT ${NATIVE_BUILD})
  set(VENV_PYTHON_EXE ${VENV_PYTHON_EXE} PARENT_SCOPE)
endif()

add_library(no-os STATIC)

target_include_directories(
        no-os
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
list(APPEND GENERATE_CONFIG_ARGS "--defconfig")
list(APPEND GENERATE_CONFIG_ARGS ${DEFAULT_DEFCONFIG})

# Add board-specific config if available
if(DEFINED NO_OS_PROJECT_NAME AND DEFINED BOARD)
  if(${NATIVE_BUILD})
    set(BOARD_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/boards/${BOARD}.conf")
  else()
    set(BOARD_CONFIG_FILE "${CMAKE_SOURCE_DIR}/boards/${BOARD}.conf")
  endif()

  if(EXISTS "${BOARD_CONFIG_FILE}")
    message(STATUS "Adding board defconfig: ${BOARD_CONFIG_FILE}")
    list(APPEND GENERATE_CONFIG_ARGS "${BOARD_CONFIG_FILE}")
  endif()
endif()

if (DEFINED PROJECT_DEFCONFIG)
  message(STATUS "Adding project defconfig: ${PROJECT_DEFCONFIG}")
  if(${NATIVE_BUILD})
    list(APPEND GENERATE_CONFIG_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/projects/${PROJECT_DEFCONFIG})
    list(APPEND GENERATE_CONFIG_ARGS --native)
  else()
    list(APPEND GENERATE_CONFIG_ARGS ${CMAKE_SOURCE_DIR}/${PROJECT_DEFCONFIG})
  endif()
endif()

target_compile_options(no-os PRIVATE -Wall)
target_link_options(no-os PRIVATE -Wl,--gc-sections)

set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${DEFAULT_DEFCONFIG}
  ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/.config
)

if(IS_FRESH_CONFIGURE)
  file(REMOVE ${CONFIG_FILE})
endif()

if(NOT EXISTS "${CONFIG_FILE}")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env
      srctree=${CMAKE_CURRENT_SOURCE_DIR}
      ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
        ${GENERATE_CONFIG_ARGS}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}
        --update
  )
else()
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env
      srctree=${CMAKE_CURRENT_SOURCE_DIR}
      ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
        ${GENERATE_CONFIG_ARGS}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

if(NOT EXISTS "${CONFIG_FILE}")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env
      srctree=${CMAKE_CURRENT_SOURCE_DIR}
      ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
        ${GENERATE_CONFIG_ARGS}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}
        --native ${NATIVE_BUILD}
  )
endif()

add_custom_command(
  OUTPUT ${KCONFIG_VARS_FILE}

  COMMAND ${CMAKE_COMMAND} -E env
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
    ${GENERATE_CONFIG_ARGS}
    --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
    --build_dir ${CMAKE_CURRENT_BINARY_DIR}

    DEPENDS ${CONFIG_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
    COMMENT "Generating CMake config variables from .config"
)

include(${KCONFIG_VARS_FILE})

add_subdirectory(projects)
add_subdirectory(libraries)
add_subdirectory(network)
add_subdirectory(util)
add_subdirectory(iio)
add_subdirectory(drivers)

add_custom_target(KconfigVars ALL
    DEPENDS ${KCONFIG_VARS_FILE}
)

add_custom_target(
  menuconfig
  COMMAND ${CMAKE_COMMAND} -E env
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    ${VENV_PYTHON_EXE} -m menuconfig
  COMMAND ${CMAKE_COMMAND} -E env
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    ${VENV_PYTHON_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
    ${GENERATE_CONFIG_ARGS}
    --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
    --build_dir ${CMAKE_CURRENT_BINARY_DIR}
  USES_TERMINAL
  VERBATIM
)

add_custom_target(
  distclean
  COMMAND make clean && rm -f ${CMAKE_CURRENT_BINARY_DIR}/.config && rm -f ${CMAKE_CURRENT_BINARY_DIR}/config.cmake
  VERBATIM
)

add_dependencies(no-os KconfigVars)

# Define the variables that will be substituted into the templates
# set(SVD_FILE_PATH "${CMAKE_SOURCE_DIR}/svd/STM32F756.svd" CACHE FILEPATH "Path to SVD file")