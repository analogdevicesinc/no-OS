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
    elseif(PLATFORM STREQUAL "altera")
        list(APPEND _backends "riscfree")
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
    if(PLATFORM STREQUAL "altera")
        # Pin the source view to the device-level directories the ad9088 Agilex
        # build actually compiles, so the IDE tree shows only what is built (not
        # whole class folders such as all of drivers/frequency). Deriving this
        # automatically from the no-os target is not possible here: the backend
        # runs from the project's CMakeLists (post_build_config), before the
        # top-level add_subdirectory(drivers) attaches driver sources, so the
        # target's SOURCES are still empty at this point. Entries that do not
        # exist for a given config are dropped by the EXISTS check in the backend.
        # Other platforms keep the broad view below.
        set(_groups
            "no-os_ad9088=${NO_OS_DIR}/drivers/rf-transceiver/ad9088"
            "no-os_axi_adc_core=${NO_OS_DIR}/drivers/axi_core/axi_adc_core"
            "no-os_axi_dac_core=${NO_OS_DIR}/drivers/axi_core/axi_dac_core"
            "no-os_axi_dmac=${NO_OS_DIR}/drivers/axi_core/axi_dmac"
            "no-os_clk_axi_clkgen=${NO_OS_DIR}/drivers/axi_core/clk_axi_clkgen"
            "no-os_axi_jesd204=${NO_OS_DIR}/drivers/axi_core/jesd204"
            "no-os_adf4030=${NO_OS_DIR}/drivers/frequency/adf4030"
            "no-os_adf4382=${NO_OS_DIR}/drivers/frequency/adf4382"
            "no-os_hmc7044=${NO_OS_DIR}/drivers/frequency/hmc7044"
            "no-os_platform_altera=${NO_OS_DIR}/drivers/platform/altera"
            "no-os_api=${NO_OS_DIR}/drivers/api"
            "no-os_jesd204=${NO_OS_DIR}/jesd204"
            "no-os_util=${NO_OS_DIR}/util"
            "no-os_include=${NO_OS_DIR}/include"
        )
    else()
        set(_groups
            "no-os_drivers=${NO_OS_DIR}/drivers"
            "no-os_include=${NO_OS_DIR}/include"
            "no-os_util=${NO_OS_DIR}/util"
        )
    endif()

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

    # ADuCM3029 Device Family Pack (CCES SDK)
    if(PLATFORM STREQUAL "aducm3029" AND DEFINED ADUCM_DFP)
        list(APPEND _groups "aducm_dfp=${ADUCM_DFP}")
    endif()

    # Altera / Nios V BSP (system.h, HAL, linker.x) generated from the HDL handoff
    if(PLATFORM STREQUAL "altera" AND DEFINED ALTERA_BSP_DIR)
        list(APPEND _groups "niosv_bsp=${ALTERA_BSP_DIR}")
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
