cmake_minimum_required(VERSION 3.27)
# set the project name

project(capi VERSION 1.0 LANGUAGES C CXX ASM)
include(FetchContent)

message(STATUS "🦨🦨🦨 No-OS build 🦨🦨🦨")

find_package (Python3 COMPONENTS Interpreter Development)
find_package(Git)

function(no_os_sources_ifdef feature_toggle source)
  if(${feature_toggle})
    message(STATUS "Adding ${source}")
    target_sources(capi_build PRIVATE ${source} ${ARGN})
  endif()
endfunction()

function(capi_update_config)
  include(${CMAKE_SOURCE_DIR}/config.cmake)
endfunction()

# Board specific defconfig
list(APPEND CONFIG_FILES ${DEFAULT_DEFCONFIG})
# list(APPEND CONFIG_FILES ${CMAKE_SOURCE_DIR}/projects/eval-adxl355-pmdz/project.conf)

set(NO_OS_DIR ${CMAKE_SOURCE_DIR})
set(CONFIG_FILE ${CMAKE_SOURCE_DIR}/.config)

add_library(capi_build STATIC)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
list(APPEND GENERATE_CONFIG_ARGS "--defconfig")
list(APPEND GENERATE_CONFIG_ARGS ${DEFAULT_DEFCONFIG})

target_compile_options(capi_build PRIVATE -Wall)
target_link_options(capi_build PRIVATE -Wl,--gc-sections)

add_subdirectory(projects)

# if (DEFINED ${CAPI_CURRENT_PROJECT})
#   list(APPEND GENERATE_CONFIG_ARGS ${NO_OS_DIR}/projects/${CAPI_CURRENT_PROJECT}/project.conf)
# endif()

set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS
  ${NO_OS_DIR}/projects/${CAPI_CURRENT_PROJECT}/project.conf
  ${DEFAULT_DEFCONFIG}
  ${NO_OS_DIR}/config.cmake
  ${NO_OS_DIR}/.config
)

message(STATUS "Using default defconfig: ${PROJECT_DEFCONFIG}")

if (DEFINED PROJECT_DEFCONFIG)
  list(APPEND GENERATE_CONFIG_ARGS ${CMAKE_SOURCE_DIR}/projects/${PROJECT_DEFCONFIG})
endif()

execute_process(
  COMMAND ${Python3_EXECUTABLE}
  ${CMAKE_SOURCE_DIR}/generate_config.py
  ${GENERATE_CONFIG_ARGS}
)

include(${CMAKE_SOURCE_DIR}/config.cmake)

add_subdirectory(network)
add_subdirectory(util)
add_subdirectory(iio)
add_subdirectory(drivers)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5 # release-1.16.0
)

FetchContent_MakeAvailable(googletest)
