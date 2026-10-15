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

# TFT SPI initialization resets SPI0. Use the RMW reset wrapper so that reset
# cannot clobber the SEMA clock used by the cross-core mailbox.
target_link_options(capi_coprocessor_max78000 PRIVATE
    -Wl,--wrap=MXC_SYS_Reset_Periph)

# Starting CPU1 gives the RISC-V core its MAX78000 SRAM banks. Keep the ARM
# data, heap, and stack in SRAM0/SRAM1 just like the existing dual-core demos;
# the default single-core linker script places the ARM stack in CPU1's SRAM3.
set(_maxim_gcc
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX${TARGET_NUM}/Source/GCC)
set(_maxim_dualcore_ld
    ${NO_OS_DIR}/drivers/platform/maxim/max${TARGET_NUM}/max${TARGET_NUM}_dualcore_arm.ld)
string(REPLACE
    "-T${_maxim_gcc}/max${TARGET_NUM}.ld"
    "-T${_maxim_dualcore_ld} -L${_maxim_gcc}"
    CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Build the camera firmware for CPU1 and embed it into the ARM image. The first
# 16 KiB of SRAM2 contains the OpenOCD work area followed by the fixed camera
# IPC window; the RISC-V linker starts private data at 0x20014000.
include(maxim_coprocessor)
set(_mxc ${MAXIM_LIBRARIES}/PeriphDrivers/Source)
set(_camera ${MAXIM_LIBRARIES}/MiscDrivers/Camera)
maxim_add_riscv_coprocessor(capi_coprocessor_max78000
    DEBUG_INFO
    OPTIMIZATION O2
    SRAM2_RESERVE 0x4000
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/camera_riscv.c
        ${_camera}/camera.c
        ${_camera}/ov7692.c
        ${_camera}/sccb.c
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/camera_ipc.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/board.h
    DEFINES
        CAMERA_OV7692
        BOARD_EVKIT_V1
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${_camera}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/capi/platform/maxim/max${TARGET_NUM}
        ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}
    NOOS_INCLUDES
        ${_camera}
    NOOS_DRIVERS
        ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_sys.c
        ${_mxc}/DMA/dma_me17.c
        ${_mxc}/DMA/dma_reva.c
        ${_mxc}/GPIO/gpio_ai85.c
        ${_mxc}/GPIO/gpio_reva.c
        ${_mxc}/GPIO/gpio_common.c
        ${_mxc}/CAMERAIF/cameraif_ai85.c
        ${_mxc}/CAMERAIF/cameraif_reva.c
        ${_mxc}/PT/pt_ai85.c
        ${_mxc}/PT/pt_reva.c
	${_mxc}/TMR/tmr_ai85.c
	${_mxc}/TMR/tmr_revb.c
	${_mxc}/TMR/tmr_common.c
        ${_mxc}/SYS/pins_ai85.c
        ${_mxc}/SYS/sys_ai85.c
        ${_mxc}/SYS/mxc_delay.c
        ${_mxc}/SYS/mxc_lock.c
        ${_mxc}/SYS/mxc_assert.c)

config_platform_sdk(capi_coprocessor_max78000)
add_flash_target(capi_coprocessor_max78000)
post_build_config(capi_coprocessor_max78000)
