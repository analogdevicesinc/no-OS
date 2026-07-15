add_executable(max78000)

# Common sources
target_sources(max78000 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}/parameters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
)

# Basic example
if(CONFIG_MAX78000_BASIC_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
endif()

# Dual-core example (ARM + RISC-V)
if(CONFIG_MAX78000_DUAL_CORE_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/dual_core_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core
    )
endif()

# Single-core dual-IMU example (ADIS + ADXL345, ARM only)
if(CONFIG_MAX78000_IMU_SINGLE_CORE_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_single_core/imu_single_core_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_single_core
    )
    # adxl345.c, adis.c, and the selected ADIS family driver are compiled via
    # CONFIG_ACCEL_ADXL345=y / CONFIG_IMU_ADIS=y / CONFIG_IMU_ADIS16xxx=y in
    # the defconfig -- adding them here again would cause duplicate symbols.
    # Redirect MXC_SYS_Reset_Periph to the RMW wrapper so MXC_SPI_Init() does
    # not clobber RST1_SIMO when reinitializing SPI0 for the second device.
    target_link_options(max78000 PRIVATE -Wl,--wrap=MXC_SYS_Reset_Periph)
endif()


target_include_directories(max78000 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
)

target_link_libraries(max78000 no-os)

# Build the RISC-V coprocessor firmware and embed it into the ARM ELF. The
# platform helper owns the two-pass link, linker script, and embed; the project
# just names the RISC-V sources. Placed after the ARM target's sources/includes
# are set so the helper's premap twin inherits them.
if(CONFIG_MAX78000_DUAL_CORE_EXAMPLE)
    include(maxim_coprocessor)
    maxim_add_riscv_coprocessor(max78000
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core/dual_core_example_riscv.c
        INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dual_core
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM})
endif()

config_platform_sdk(max78000)
add_flash_target(max78000)
post_build_config(max78000)
