cmake_minimum_required(VERSION 3.27)

# list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(no-os VERSION 1.0 LANGUAGES C CXX ASM)
include(FetchContent)

if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/CMakeCache.txt")
  set(IS_FRESH_CONFIGURE TRUE)
else()
  set(IS_FRESH_CONFIGURE FALSE)
endif()

message(STATUS "🦨🦨🦨 No-OS build 🦨🦨🦨")

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
    target_include_directories(no-os PRIVATE ${include_dir})
  endif()
endfunction()

# Board specific defconfig
list(APPEND CONFIG_FILES ${DEFAULT_DEFCONFIG})

message(STATUS "Project defconfig: ${PROJECT_DEFCONFIG}")

set(NO_OS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/.config)
set(KCONFIG_VARS_FILE ${CMAKE_CURRENT_BINARY_DIR}/config.cmake)
set(CREATE_DOTCONFIG_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/create_dotconfig.cmake)

add_library(no-os STATIC)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
list(APPEND GENERATE_CONFIG_ARGS "--defconfig")
list(APPEND GENERATE_CONFIG_ARGS ${DEFAULT_DEFCONFIG})
if (DEFINED PROJECT_DEFCONFIG)
  message(ERROR "Adding project defconfig")
  list(APPEND GENERATE_CONFIG_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/projects/${PROJECT_DEFCONFIG})
endif()

target_compile_options(no-os PRIVATE -Wall)
target_link_options(no-os PRIVATE -Wl,--gc-sections)

add_subdirectory(projects)

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
      ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
        ${GENERATE_CONFIG_ARGS}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}
        --update
  )
else()
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env
      srctree=${CMAKE_CURRENT_SOURCE_DIR}
      ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
        ${GENERATE_CONFIG_ARGS}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

if(NOT EXISTS "${CONFIG_FILE}")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env
      srctree=${CMAKE_CURRENT_SOURCE_DIR}
      ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
        ${GENERATE_CONFIG_ARGS}
        --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
        --build_dir ${CMAKE_CURRENT_BINARY_DIR}
  )
endif()

add_custom_command(
  OUTPUT ${KCONFIG_VARS_FILE}

  COMMAND ${CMAKE_COMMAND} -E env
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
    ${GENERATE_CONFIG_ARGS}
    --root_dir ${CMAKE_CURRENT_SOURCE_DIR}
    --build_dir ${CMAKE_CURRENT_BINARY_DIR}

    DEPENDS ${CONFIG_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
    COMMENT "Generating CMake config variables from .config"
)

include(${KCONFIG_VARS_FILE})

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
    ${Python3_EXECUTABLE} -m menuconfig
  COMMAND ${CMAKE_COMMAND} -E env
    srctree=${CMAKE_CURRENT_SOURCE_DIR}
    ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_config.py
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