target_compile_definitions(no-os PUBLIC -DXILINX_PLATFORM)

# Arch-conditionality (PS vs PL peripherals, PS7 vs PSU) is handled inside the
# driver sources via #ifdef on the BSP-provided XPAR_* / header guards, so the
# sources are added whenever their Kconfig symbol is on.
no_os_sources_ifdef(CONFIG_UART_XILINX     ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_uart.c)
no_os_sources_ifdef(CONFIG_IRQ_XILINX      ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_irq.c)
no_os_sources_ifdef(CONFIG_SPI_XILINX      ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_spi.c)
no_os_sources_ifdef(CONFIG_SPI_XILINX      ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_spi_pl.c)
no_os_sources_ifdef(CONFIG_I2C_XILINX      ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_i2c.c)
no_os_sources_ifdef(CONFIG_GPIO_XILINX     ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_gpio.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_XILINX ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_gpio_irq.c)
no_os_sources_ifdef(CONFIG_TIMER_XILINX    ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_timer.c)

# xilinx_delay.c and xilinx_axi_io.c are used unconditionally by the platform.
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/xilinx_delay.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
