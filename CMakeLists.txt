cmake_minimum_required(VERSION 3.27)
# set the project name

project(capi VERSION 1.0 LANGUAGES C CXX ASM)
include(FetchContent)

message(STATUS "🦨🦨🦨 No-OS build 🦨🦨🦨")

find_package (Python3 COMPONENTS Interpreter Development)
find_package(Git)

function(no_os_sources_ifdef feature_toggle source)
  if(${feature_toggle})
    target_sources(capi_build PRIVATE ${source} ${ARGN})
  endif()
endfunction()

list(APPEND GENERATE_CONFIG_ARGS ${CMAKE_SOURCE_DIR})
list(APPEND GENERATE_CONFIG_ARGS ${DEFAULT_DEFCONFIG})

# list(APPEND CONFIG_FILES ${CMAKE_SOURCE_DIR}/generate_config.py)

execute_process(
        COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/generate_config.py
        ${GENERATE_CONFIG_ARGS}
)

# add_custom_command(
#         OUTPUT ${CMAKE_BINARY_DIR}/.config
#         COMMAND ${Python3_EXECUTABLE}
#         ${CMAKE_SOURCE_DIR}/generate_config.py
#         ${GENERATE_CONFIG_ARGS}
#         DEPENDS ${CMAKE_SOURCE_DIR}/generate_config.py ${CONFIG_FILES}
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
# )

# add_custom_target(
#         generate_config ALL
#         DEPENDS ${CMAKE_BINARY_DIR}/.config
# )

set(CONFIG_FILE ${CMAKE_SOURCE_DIR}/.config)
configure_file(${CONFIG_FILE} ${CMAKE_BINARY_DIR}/.config COPYONLY)

include(config.cmake)

add_library(capi_build STATIC)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(capi_build PRIVATE -Wall)
target_link_options(capi_build PRIVATE -Wl,--gc-sections)

add_subdirectory(drivers)
add_subdirectory(util)
add_subdirectory(iio)
add_subdirectory(network)
add_subdirectory(projects)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5 # release-1.16.0
)

FetchContent_MakeAvailable(googletest)
