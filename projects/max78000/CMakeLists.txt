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

# Dual-core IMU example (ADIS16577 on RISC-V, ADXL345 on ARM)
if(CONFIG_MAX78000_IMU_DUAL_CORE_EXAMPLE)
    target_sources(max78000 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core/imu_dual_core_example.c
    )
    target_include_directories(max78000 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core
    )
    # adxl345.c is compiled through CONFIG_ACCEL_ADXL345=y in the defconfig.
    # Redirect MXC_SYS_Reset_Periph to the RMW wrapper in imu_dual_core_example.c
    # so the ARM's adxl345_init() -> MXC_SPI_Init() does not clobber RST1_SMPHR,
    # which would destroy the hardware semaphore used as the SPI bus lock.
    target_link_options(max78000 PRIVATE -Wl,--wrap=MXC_SYS_Reset_Periph)
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

if(CONFIG_MAX78000_IMU_DUAL_CORE_EXAMPLE)
    include(maxim_coprocessor)

    # no-OS + Maxim + MXC + ADIS sources cross-compiled into the RISC-V image so
    # the coprocessor drives the ADIS16577 through the real no-OS driver stack.
    set(_mxc ${MAXIM_LIBRARIES}/PeriphDrivers/Source)
    maxim_add_riscv_coprocessor(max78000
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core/imu_dual_core_example_riscv.c
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core/imu_ipc_shared.h
        INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/imu_dual_core
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}
        NOOS_INCLUDES
            ${CMAKE_SOURCE_DIR}/drivers/imu
        NOOS_DRIVERS
            # no-OS API layer
            ${CMAKE_SOURCE_DIR}/drivers/api/no_os_spi.c
            ${CMAKE_SOURCE_DIR}/drivers/api/no_os_gpio.c
            ${CMAKE_SOURCE_DIR}/drivers/api/no_os_dma.c
            ${CMAKE_SOURCE_DIR}/drivers/api/no_os_irq.c
            ${CMAKE_SOURCE_DIR}/util/no_os_util.c
            ${CMAKE_SOURCE_DIR}/util/no_os_list.c
            ${CMAKE_SOURCE_DIR}/util/no_os_mutex.c
            ${CMAKE_SOURCE_DIR}/util/no_os_alloc.c
            # Maxim platform drivers
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_spi.c
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_gpio_spi.c
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_gpio.c
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_delay.c
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/max${TARGET_NUM}/maxim_sys.c
            ${CMAKE_SOURCE_DIR}/drivers/platform/maxim/common/maxim_dma.c
            # MXC PeriphDrivers (RV32-compiled)
            ${_mxc}/SPI/spi_me17.c
            ${_mxc}/SPI/spi_reva1.c
            ${_mxc}/GPIO/gpio_me17.c
            ${_mxc}/GPIO/gpio_reva.c
            ${_mxc}/GPIO/gpio_common.c
            ${_mxc}/DMA/dma_me17.c
            ${_mxc}/DMA/dma_reva.c
            ${_mxc}/SYS/sys_ai85.c
            ${_mxc}/SYS/pins_ai85.c
            ${_mxc}/SYS/mxc_delay.c
            ${_mxc}/SYS/mxc_lock.c
            ${_mxc}/SYS/mxc_assert.c
            # ADIS device driver
            ${CMAKE_SOURCE_DIR}/drivers/imu/adis.c
            ${CMAKE_SOURCE_DIR}/drivers/imu/adis1657x.c)
endif()

config_platform_sdk(max78000)
add_flash_target(max78000)
post_build_config(max78000)
