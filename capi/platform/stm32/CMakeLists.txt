# STM32 CAPI backends. Gated on the neutral CONFIG_CAPI_<MODULE> symbol.

no_os_sources_ifdef(CONFIG_CAPI_UART  ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_uart.c)
no_os_sources_ifdef(CONFIG_CAPI_ALLOC ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_alloc.c)
no_os_sources_ifdef(CONFIG_CAPI_TIME  ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_time.c)
no_os_sources_ifdef(CONFIG_CAPI_GPIO  ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_gpio.c)
no_os_sources_ifdef(CONFIG_CAPI_IRQ   ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_irq.c)
no_os_sources_ifdef(CONFIG_CAPI_SPI   ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_spi.c)
no_os_sources_ifdef(CONFIG_CAPI_TIMER ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_timer.c)
no_os_sources_ifdef(CONFIG_CAPI_I2C   ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_i2c.c)
no_os_sources_ifdef(CONFIG_CAPI_DMA   ${CMAKE_CURRENT_SOURCE_DIR}/stm32_capi_dma.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
