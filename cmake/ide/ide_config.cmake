# --- IDE Abstraction Layer ---
#
# Entry point for IDE project file generation. Provides:
#   ide_generate(PROJECT_TARGET)     - Main entry: builds source groups, dispatches to backends
#   ide_get_backends(OUT_VAR)        - Returns list of IDE backends for the current platform
#   ide_build_source_groups()        - Builds IDE_SOURCE_GROUPS list of name=path pairs

set(IDE_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Determine which IDE backends to generate for the current platform.
# Platforms get a default set, overridable via -DIDE_BACKENDS="vscode;keil"
function(ide_get_backends OUT_VAR)
    set(_backends "vscode")
    if(PLATFORM STREQUAL "stm32")
        list(APPEND _backends "stm32cubeide")
    elseif(PLATFORM STREQUAL "xilinx")
        list(APPEND _backends "vitis")
    endif()
    # User override
    if(DEFINED IDE_BACKENDS)
        set(_backends ${IDE_BACKENDS})
    endif()
    set(${OUT_VAR} ${_backends} PARENT_SCOPE)
endfunction()

# Build a list of source group entries (name=path pairs) that represent the
# "code view" every IDE backend should expose. Driven by Kconfig variables
# and known no-OS directory structure.
function(ide_build_source_groups)
    set(_groups
        "no-os_drivers=${NO_OS_DIR}/drivers"
        "no-os_include=${NO_OS_DIR}/include"
        "no-os_util=${NO_OS_DIR}/util"
    )

    # Project source directory
    if(DEFINED NO_OS_PROJECT_NAME)
        list(APPEND _groups "project_src=${CMAKE_SOURCE_DIR}/projects/${NO_OS_PROJECT_NAME}/src")
    endif()

    if(CONFIG_IIO)
        list(APPEND _groups "no-os_iio=${NO_OS_DIR}/iio")
    endif()

    if(CONFIG_FREERTOS AND DEFINED FREERTOS_SOURCE_DIR)
        list(APPEND _groups "freertos=${FREERTOS_SOURCE_DIR}")
    endif()

    if(CONFIG_LWIP AND DEFINED LWIP_SOURCE_DIR)
        list(APPEND _groups "lwip=${LWIP_SOURCE_DIR}")
    endif()

    if(CONFIG_CORDIO AND DEFINED CORDIO_SOURCE_DIR)
        list(APPEND _groups "cordio=${CORDIO_SOURCE_DIR}")
    endif()

    # STM32 HAL (CubeMX-generated)
    if(PLATFORM STREQUAL "stm32" AND DEFINED NO_OS_PROJECT_NAME)
        list(APPEND _groups "stm32_hal=${CMAKE_BINARY_DIR}/projects/${NO_OS_PROJECT_NAME}")
    endif()

    # Maxim SDK
    if(PLATFORM STREQUAL "maxim" AND DEFINED MAXIM_LIBRARIES)
        list(APPEND _groups "maxim_sdk=${MAXIM_LIBRARIES}")
    endif()

    set(IDE_SOURCE_GROUPS ${_groups} PARENT_SCOPE)
endfunction()

# Main entry point: build source groups, then dispatch to each backend.
function(ide_generate PROJECT_TARGET)
    ide_build_source_groups()
    ide_get_backends(_backends)
    foreach(_be ${_backends})
        include(${IDE_DIR}/backends/${_be}.cmake)
        cmake_language(CALL ide_${_be}_configure ${PROJECT_TARGET})
        cmake_language(CALL ide_${_be}_post_build ${PROJECT_TARGET})
    endforeach()
endfunction()
