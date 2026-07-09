add_executable(adrv9009)

# Select the headless entry-point and companion sources by Xilinx processor arch.
#
# XILINX_ARCH is set by drivers/platform/xilinx/toolchain.cmake (value like
# "ps7_cortexa9_0", "psu_cortexa53_0", "sys_mb_0") before CMakeLists runs.
#
# cortexa9/cortexa53 (zc706/zcu102 standard FMC card) — headless_arm.c
#   headless_arm.c uses the raw Talise vendor API via the arm app helpers
#   (app_clocking/jesd/transceiver/talise).  It also defines the two
#   settings_get functions (adrv9009_spi_settings_get and
#   adrv9009_initial_settings_get) that adrv9009.c in the driver library
#   calls — these were missing from the original file causing undefined
#   reference errors when CONFIG_RF_TRANSCEIVER_ADRV9009=y compiled adrv9009.c.
#
#   IMPORTANT: the variant confs intentionally do NOT set
#   CONFIG_RF_TRANSCEIVER_ADRV9009=y for arm/cortexa53 standard FMC builds.
#   Enabling that Kconfig would compile adrv9009.c alongside app_talise.c —
#   both include talise_arm_binary.h / talise_stream_binary.h which define
#   non-static global arrays (armBinary[], streamBinary[]) causing
#   multiple-definition linker errors.  Instead the raw Talise vendor API
#   sources (api/*.c) are added directly to the no-os library target here,
#   mirroring the legacy arm_src.mk explicit source list.
#
# microblaze (kcu105) — headless_mb.c
#   headless_mb.c uses the adrv9009_init() wrapper + JESD FSM; it defines
#   the settings_get functions.  CONFIG_RF_TRANSCEIVER_ADRV9009=y compiles
#   the full talise/ tree (including adrv9009.c) from the library.
#
# ZU11EG SOM / FMCOMMS8 dual-device (adrv2crr-fmc + adrv9009-zu11eg) —
#   would need headless_zu.c + ZU11EG / FMCOMMS8_ZCU102 define, but those
#   builds use XSA files with XPAR_AXI_ADRV9009_SOM_* / _FMC_* names and
#   are NOT covered by the zcu102 board presently in scope.
#
if(XILINX_ARCH MATCHES "sys_mb")
    # MicroBlaze: use the adrv9009_init() wrapper path
    # CONFIG_RF_TRANSCEIVER_ADRV9009=y should be set for microblaze builds
    # (it adds adrv9009.c which defines settings_get and the full talise tree)
    target_sources(adrv9009 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/headless_mb.c
    )
    target_compile_definitions(adrv9009 PRIVATE PLATFORM_MB)
else()
    # ARM targets: cortexa9 (zc706) and cortexa53 (zcu102 standard FMC card)
    # Both use the same single-device FMC design path (XPAR_AXI_ADRV9009_* names).
    # CONFIG_RF_TRANSCEIVER_ADRV9009 is NOT set for these builds (see above);
    # the raw Talise vendor API sources are added here directly.
    set(_TALISE_API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/rf-transceiver/talise/api)
    target_sources(no-os PRIVATE
        ${_TALISE_API_DIR}/talise_agc.c
        ${_TALISE_API_DIR}/talise_arm.c
        ${_TALISE_API_DIR}/talise.c
        ${_TALISE_API_DIR}/talise_cals.c
        ${_TALISE_API_DIR}/talise_error.c
        ${_TALISE_API_DIR}/talise_gpio.c
        ${_TALISE_API_DIR}/talise_hal.c
        ${_TALISE_API_DIR}/talise_jesd204.c
        ${_TALISE_API_DIR}/talise_radioctrl.c
        ${_TALISE_API_DIR}/talise_rx.c
        ${_TALISE_API_DIR}/talise_tx.c
        ${_TALISE_API_DIR}/talise_user.c
    )
    target_include_directories(no-os PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/rf-transceiver/talise/api
        ${CMAKE_CURRENT_SOURCE_DIR}/../../drivers/rf-transceiver/talise/firmware
    )
    target_sources(adrv9009 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/headless_arm.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_clocking.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_jesd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_transceiver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app/app_talise.c
    )
endif()

# Sources common to all arches
target_sources(adrv9009 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/adi_hal/no_os_hal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/profiles/tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88/talise_config.c
)

# demo variant — no extra flags; standard bring-up, no DMA capture, no IIO
if(CONFIG_ADRV9009_DEMO_EXAMPLE)
    # No additional sources or defines needed
endif()

# adrv9008-1 variant — build for ADRV9008-1 (TX-only sub-variant)
if(CONFIG_ADRV9009_ADRV9008_1_EXAMPLE)
    target_compile_definitions(adrv9009 PRIVATE ADRV9008_1)
endif()

# adrv9008-2 variant — build for ADRV9008-2 (RX-only sub-variant)
if(CONFIG_ADRV9009_ADRV9008_2_EXAMPLE)
    target_compile_definitions(adrv9009 PRIVATE ADRV9008_2)
endif()

# dma_example variant — DMA-based data capture/playback
if(CONFIG_ADRV9009_DMA_EXAMPLE)
    target_compile_definitions(adrv9009 PRIVATE DMA_EXAMPLE)
endif()

# iio variant — IIO/IIOD support over UART (IIOD=y in legacy Make)
if(CONFIG_ADRV9009_IIO_EXAMPLE)
    target_compile_definitions(adrv9009 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(adrv9009 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/adi_hal
)

# The talise driver (adrv9009.c in the no-os library) directly #includes
# talise_config.h, app_config.h, parameters.h, and adi_hal.h which live in
# the project source tree (not in the driver tree).  They MUST be on the
# no-os library include path so the driver compiles correctly.
# - talise_config.h / talise_config_ad9528.h: from the profile directory
# - app_config.h / parameters.h: from src/app/
# - adi_hal.h / common.h: from src/devices/adi_hal/
target_include_directories(no-os PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/adi_hal
    ${CMAKE_CURRENT_SOURCE_DIR}/profiles/tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88
)

config_platform_sdk(adrv9009)
add_flash_target(adrv9009)
post_build_config(adrv9009)
target_link_libraries(adrv9009 no-os)
