add_executable(ad9371)

# Select the entry-point and companion sources by Xilinx processor arch.
#
# XILINX_ARCH is set by drivers/platform/xilinx/toolchain.cmake (value like
# "ps7_cortexa9_0", "psu_cortexa53_0", "sys_mb_0") before CMakeLists runs.
#
# cortexa9/cortexa53 (zc706/zcu102) — headless.c uses the raw Mykonos vendor
# API directly (mykonos.c, mykonos_gpio.c, etc.) plus the adi_hal common.c
# HAL shim.  The mykonos device sources are in the project tree, not in any
# no-OS library driver, so they are compiled into the no-os library target
# here to avoid the ODR / multiple-definition issue that arises when a wrapper
# CONFIG enables the same symbols.
#
# IMPORTANT: the variant confs intentionally do NOT set
# CONFIG_RF_TRANSCEIVER_AD9371=y for arm/cortexa53 builds.  All mykonos
# vendor API sources are added directly to the no-os target, mirroring what
# the legacy arm src.mk did with the explicit SRCS list.
#
# MicroBlaze (kcu105) — same headless.c is used; there is no separate
# headless_mb.c for ad9371 (the single headless.c already gates the
# Xil_ICacheEnable/Disable calls behind #ifdef XILINX_PLATFORM and uses
# #ifdef XPAR_XUARTLITE_NUM_INSTANCES for UART selection).
# CONFIG_RF_TRANSCEIVER_AD9371=y is NOT set for MB either; the same explicit
# src approach is used to stay consistent and avoid any duplicate-symbol risk.

set(_MYK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/mykonos)
set(_AD9528_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/ad9528)
set(_HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/adi_hal)
set(_PROFILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/profiles/tx_bw200_orx_bw200_rx_bw100)

# Mykonos vendor API sources and the adi_hal HAL shim — go on no-os target so
# the driver headers (#included by headless.c and the hal) are resolved at the
# same scope.
target_sources(no-os PRIVATE
    ${_MYK_DIR}/mykonos.c
    ${_MYK_DIR}/mykonos_gpio.c
    ${_MYK_DIR}/mykonos_debug/mykonos_dbgjesd.c
    ${_MYK_DIR}/mykonosMmap.c
    ${_MYK_DIR}/mykonos_user.c
    ${_AD9528_DIR}/ad9528.c
    ${_HAL_DIR}/common.c
    ${_PROFILE_DIR}/myk.c
    ${_PROFILE_DIR}/myk_ad9528init.c
)

# The Mykonos driver and HAL headers #included by headless.c, common.c, myk.c:
# - mykonos.h, t_mykonos.h, mykonos_gpio.h, t_mykonos_gpio.h, mykonos_macros.h,
#   mykonos_user.h, mykonos_version.h, mykonos_debug/*
# - ad9528.h, t_ad9528.h
# - common.h, parameters.h (parameters.h also pulls in app_config.h)
# - myk.h (profile-specific device config)
# - Mykonos_M3.h (firmware blob header)
# All must be on the no-os include path so they resolve when the sources above
# compile into the no-os library.
target_include_directories(no-os PUBLIC
    ${_MYK_DIR}
    ${_MYK_DIR}/mykonos_debug
    ${_AD9528_DIR}
    ${_HAL_DIR}
    ${_PROFILE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/firmware
)

# Project-local entry point — headless.c
target_sources(ad9371 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/headless.c
)

# MicroBlaze: emit PLATFORM_MB so headless.c gates dcache helpers correctly
if(XILINX_ARCH MATCHES "sys_mb")
    target_compile_definitions(ad9371 PRIVATE PLATFORM_MB)
endif()

# demo variant — standard bring-up, no extra flags
if(CONFIG_AD9371_DEMO_EXAMPLE)
    # No additional sources or defines needed
endif()

# dma_example variant — DMA-based data capture/playback
if(CONFIG_AD9371_DMA_EXAMPLE)
    target_compile_definitions(ad9371 PRIVATE DMA_EXAMPLE)
endif()

# iio variant — IIO/IIOD support over UART (IIOD=y in legacy Make)
if(CONFIG_AD9371_IIO_EXAMPLE)
    target_compile_definitions(ad9371 PRIVATE IIO_SUPPORT)
endif()

target_include_directories(ad9371 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

config_platform_sdk(ad9371)
add_flash_target(ad9371)
post_build_config(ad9371)
target_link_libraries(ad9371 no-os)
