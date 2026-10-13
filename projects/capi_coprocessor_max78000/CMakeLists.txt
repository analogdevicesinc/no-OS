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

# Reserve OpenOCD and IPC storage below CPU1's private SRAM2 data.
include(maxim_coprocessor)
maxim_add_riscv_coprocessor(capi_coprocessor_max78000
    DEFCONFIG ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/project.conf
    DEBUG_INFO
    OPTIMIZATION O2
    SRAM2_RESERVE 0x4000
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/camera_riscv.c
    DEFINES BOARD_EVKIT_V1
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

config_platform_sdk(capi_coprocessor_max78000)
add_flash_target(capi_coprocessor_max78000)
post_build_config(capi_coprocessor_max78000)
