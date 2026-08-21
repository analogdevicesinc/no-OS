# Platform definitions
target_compile_definitions(no-os PUBLIC -DLINUX_PLATFORM)

no_os_sources_ifdef(CONFIG_SPI_LINUX ${CMAKE_CURRENT_SOURCE_DIR}/linux_spi.c)
no_os_sources_ifdef(CONFIG_GPIO_LINUX ${CMAKE_CURRENT_SOURCE_DIR}/linux_gpio.c)
no_os_sources_ifdef(CONFIG_I2C_LINUX ${CMAKE_CURRENT_SOURCE_DIR}/linux_i2c.c)
no_os_sources_ifdef(CONFIG_UART_LINUX ${CMAKE_CURRENT_SOURCE_DIR}/linux_uart.c)
no_os_sources_ifdef(CONFIG_TIMER_LINUX ${CMAKE_CURRENT_SOURCE_DIR}/linux_timer.c)
no_os_sources_ifdef(CONFIG_AXI_IO_LINUX ${CMAKE_CURRENT_SOURCE_DIR}/linux_axi_io.c)

# Delay is implemented against usleep/clock_gettime and is always available on
# the Linux platform; no vendor peripheral is required for it.
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/linux_delay.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
