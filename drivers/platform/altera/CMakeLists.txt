if(NOT CONFIG_ALTERA_PLATFORM_NIOSV)
    return()
endif()

target_compile_definitions(no-os PRIVATE CONFIG_ALTERA_PLATFORM_NIOSV=1)

# altera_delay.c and altera_axi_io.c are used unconditionally by the platform.
# altera_axi_io.c provides no_os_axi_io_read/write, needed by every axi_core
# driver (axi_dmac, spi_engine, axi_adc_core, ...).
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/altera_delay.c)
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/altera_axi_io.c)

# The remaining sources implement the legacy Altera Avalon-SOPC peripheral
# drivers. They depend on Avalon IP-core headers (e.g.
# altera_avalon_spi_regs.h) that are only present in a BSP generated for a
# hardware design that instantiates the matching IP core, so they are opt-in
# per driver rather than pulled in unconditionally.
no_os_sources_ifdef(CONFIG_GPIO_ALTERA ${CMAKE_CURRENT_SOURCE_DIR}/altera_gpio.c)
no_os_sources_ifdef(CONFIG_I2C_ALTERA  ${CMAKE_CURRENT_SOURCE_DIR}/altera_i2c.c)
no_os_sources_ifdef(CONFIG_SPI_ALTERA  ${CMAKE_CURRENT_SOURCE_DIR}/altera_spi.c)
