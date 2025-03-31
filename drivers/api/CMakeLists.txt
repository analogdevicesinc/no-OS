no_os_sources_ifdef(CONFIG_I2C ${CMAKE_CURRENT_SOURCE_DIR}/no_os_i2c.c)
no_os_sources_ifdef(CONFIG_SPI ${CMAKE_CURRENT_SOURCE_DIR}/no_os_spi.c)
no_os_sources_ifdef(CONFIG_UART ${CMAKE_CURRENT_SOURCE_DIR}/no_os_uart.c)
no_os_sources_ifdef(CONFIG_IRQ ${CMAKE_CURRENT_SOURCE_DIR}/no_os_irq.c)