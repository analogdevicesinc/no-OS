add_executable(capi_coprocessor_max78000)

target_sources(capi_coprocessor_max78000 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera_overlay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/camera_tft.c
)

target_include_directories(capi_coprocessor_max78000 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MAXIM_LIBRARIES}/MiscDrivers/Display
    ${MAXIM_LIBRARIES}/MiscDrivers/Touchscreen
)

target_link_libraries(capi_coprocessor_max78000 no-os)

# Build the camera firmware for CPU1 with its own no-OS configuration and embed
# it into the ARM image. The first 16 KiB of SRAM2 contains the OpenOCD work area
# followed by the fixed camera IPC window; the RISC-V linker starts private data
# at 0x20014000.
include(maxim_coprocessor)
maxim_add_riscv_coprocessor(capi_coprocessor_max78000
    DEFCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/riscv/demo.conf
    DEBUG_INFO
    OPTIMIZATION O2
    SRAM2_RESERVE 0x4000
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/camera_riscv.c
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/camera_ipc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/board.h
    DEFINES BOARD_EVKIT_V1
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

config_platform_sdk(capi_coprocessor_max78000)
add_flash_target(capi_coprocessor_max78000)
post_build_config(capi_coprocessor_max78000)
