add_executable(ad9088)

# Software-in-the-loop build (host, no board): -DAD9088_SIM=ON with the
# native-i386 preset builds the Agilex 5 variant of the project for the PC and
# replaces the hardware with the models in src/sim/. The Altera platform
# sources are reused; OFF by default.
option(AD9088_SIM "Build against the src/sim hardware models (host, no board)" OFF)
set(AD9088_PLATFORM_DIR ${PLATFORM})
if(AD9088_SIM)
    set(AD9088_PLATFORM_DIR altera)
endif()

target_sources(ad9088 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/common_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${AD9088_PLATFORM_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${AD9088_PLATFORM_DIR}/parameters.c
)

# main.c selects the example with a bare #ifdef, so the Kconfig choice is
# translated back into the macro the sources already use.
if(CONFIG_AD9088_BASIC_EXAMPLE)
    target_sources(ad9088 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic/basic_example.c
    )
    target_include_directories(ad9088 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/basic
    )
    target_compile_definitions(ad9088 PRIVATE BASIC_EXAMPLE)
endif()

if(CONFIG_AD9088_DMA_EXAMPLE)
    target_sources(ad9088 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma/dma_example.c
    )
    target_include_directories(ad9088 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/dma
    )
    target_compile_definitions(ad9088 PRIVATE DMA_EXAMPLE)
endif()

if(CONFIG_AD9088_IIO_EXAMPLE)
    target_sources(ad9088 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio/iio_example.c
    )
    target_include_directories(ad9088 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/examples/iio
    )
    target_compile_definitions(ad9088 PRIVATE IIO_EXAMPLE)
endif()

# src/platform (not just src/platform/${PLATFORM}) is required:
# platform_includes.h reaches the parameters header as "xilinx/parameters.h".
target_include_directories(ad9088 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${AD9088_PLATFORM_DIR}
)

if(AD9088_SIM)
    target_sources(ad9088 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sim/sim_platform.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sim/apollo_model.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sim/sim_uart.c
    )
    # Nios V HAL header stand-ins first, then the Altera init-param headers.
    target_include_directories(ad9088 BEFORE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sim/hal
        ${NO_OS_DIR}/drivers/platform/altera
    )
    target_compile_definitions(ad9088 PRIVATE
        AD9088_SIM
        altera_spi_ops=sim_spi_ops
        altera_gpio_ops=sim_gpio_ops
        altera_uart_ops=sim_uart_ops
        altera_irq_ops=sim_irq_ops
    )
    # iio_app.c (IIO example) is in the no-OS library and includes the
    # Altera UART/IRQ headers and names the Altera IRQ ops directly.
    target_include_directories(no-os PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/sim/hal
        ${NO_OS_DIR}/drivers/platform/altera
    )
    target_compile_definitions(no-os PRIVATE altera_irq_ops=sim_irq_ops)
    target_link_options(ad9088 PRIVATE
        -Wl,--wrap=no_os_axi_io_read
        -Wl,--wrap=no_os_axi_io_write
    )
    # GCC 15 defaults to C23, where the linux-userspace platform's
    # "int32_t (*)()" casts no longer compile; build as C17.
    target_compile_options(no-os PRIVATE -std=gnu17)
    target_compile_options(ad9088 PRIVATE -std=gnu17)
    # Firmware / profile blobs are linked as host objects.
    set(NO_OS_OBJCOPY_BIN_FORMAT "elf32-i386")
    set(NO_OS_OBJCOPY_BIN_ARCH "i386")
endif()

# Device profile and signed core firmware, linked in as binary blobs. The
# driver owns the image list and the symbol names; PROFILE overrides the
# default use-case profile.
include(${NO_OS_DIR}/drivers/rf-transceiver/ad9088/ad9088_firmware.cmake)
ad9088_add_firmware(ad9088 PROFILE "${CONFIG_AD9088_PROFILE}")

# ad9088's app and the Apollo/xilinx drivers it uses are SDT-clean, so opt in
# to the Vitis 2025+ SDT ABI when the BSP is SDT. Other projects keep the
# classic ABI until individually validated (see config_xilinx_sdk).
set(NOOS_XILINX_SDT ON)

config_platform_sdk(ad9088)
add_flash_target(ad9088)
post_build_config(ad9088)
target_link_libraries(ad9088 no-os)
