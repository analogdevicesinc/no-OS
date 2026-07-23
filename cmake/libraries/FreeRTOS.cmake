# Source tree resolved by resolve_library_source (override -> managed clone).
include(LibraryCacheUtils)

# Default version if not set via Kconfig
if(NOT DEFINED CONFIG_FREERTOS_VERSION OR "${CONFIG_FREERTOS_VERSION}" STREQUAL "")
    set(CONFIG_FREERTOS_VERSION "V11.2.0")
endif()

message(STATUS "FreeRTOS requested version: ${CONFIG_FREERTOS_VERSION}")

# Resolve library source using hybrid priority chain
resolve_library_source(
    freertos
    "${CONFIG_FREERTOS_VERSION}"
    "${NO_OS_DIR}/libraries/FreeRTOS-Kernel"
    "https://github.com/FreeRTOS/FreeRTOS-Kernel.git"
    FREERTOS_SOURCE_DIR
    FREERTOS_BINARY_DIR
)

# Configure FreeRTOS build options
set(FREERTOS_HEAP ${CONFIG_FREERTOS_HEAP_ALLOCATOR} CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
if(CMAKE_CROSSCOMPILING)
    set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
endif()

# Create freertos_config interface library for FreeRTOSConfig.h
add_library(freertos_config INTERFACE)

if(NOT CONFIG_FREERTOS_CONF_PATH)
    target_include_directories(freertos_config SYSTEM
        INTERFACE
        ${NO_OS_DIR}/drivers/platform/freeRTOS
    )
else()
    # Handle CONFIG_FREERTOS_CONF_PATH - make it relative to appropriate base directory
    # For native builds, use NO_OS_DIR; for external builds, use CMAKE_SOURCE_DIR
    if("${CMAKE_SOURCE_DIR}" STREQUAL "${NO_OS_DIR}")
        # Native build - path relative to NO_OS_DIR
        set(_freertos_conf_abs "${NO_OS_DIR}/${CONFIG_FREERTOS_CONF_PATH}")
    else()
        # External build - path relative to CMAKE_SOURCE_DIR
        set(_freertos_conf_abs "${CMAKE_SOURCE_DIR}/${CONFIG_FREERTOS_CONF_PATH}")
    endif()

    # CONFIG_FREERTOS_CONF_PATH follows the legacy Makefile convention and may point
    # directly at FreeRTOSConfig.h. target_include_directories() needs the directory
    # that contains the header, so resolve to it when a file path is given.
    if(EXISTS "${_freertos_conf_abs}" AND NOT IS_DIRECTORY "${_freertos_conf_abs}")
        get_filename_component(_freertos_conf_abs "${_freertos_conf_abs}" DIRECTORY)
    endif()

    target_include_directories(freertos_config SYSTEM
        INTERFACE
        ${_freertos_conf_abs}
    )
endif()

# If a prior include added freertos_kernel, reuse it
# Otherwise, we need to add the subdirectory
if(NOT TARGET freertos_kernel)
    add_subdirectory("${FREERTOS_SOURCE_DIR}" "${FREERTOS_BINARY_DIR}" EXCLUDE_FROM_ALL)
endif()

target_link_libraries(no-os PUBLIC freertos_kernel)

target_compile_definitions(no-os PUBLIC FREERTOS)

message(STATUS "FreeRTOS configured from: ${FREERTOS_SOURCE_DIR}")
