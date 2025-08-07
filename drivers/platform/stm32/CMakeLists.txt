no_os_sources_ifdef(CONFIG_I2C_STM32 ${CMAKE_CURRENT_SOURCE_DIR}/stm32_i2c.c)
no_os_sources_ifdef(CONFIG_UART_STM32 ${CMAKE_CURRENT_SOURCE_DIR}/stm32_uart.c)
no_os_sources_ifdef(CONFIG_UART_STDIO_STM32 ${CMAKE_CURRENT_SOURCE_DIR}/stm32_uart_stdio.c)
no_os_sources_ifdef(CONFIG_SPI_STM32 ${CMAKE_CURRENT_SOURCE_DIR}/stm32_spi.c)
no_os_sources_ifdef(CONFIG_IRQ_STM32 ${CMAKE_CURRENT_SOURCE_DIR}/stm32_irq.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_STM32 ${CMAKE_CURRENT_SOURCE_DIR}/stm32_gpio_irq.c)
no_os_sources_ifdef(CONFIG_GPIO_STM32 ${CMAKE_CURRENT_SOURCE_DIR}/stm32_gpio.c)

if(NOT ${CONFIG_FREERTOS})
        target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stm32_delay.c)
endif()

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(no-os PUBLIC -DSTM32_PLATFORM=1)

if(OPENOCD_PATH)
  set(OPENOCD_INTERFACE "interface/stlink-dap.cfg")
  set(OPENOCD_CHIPNAME "STM32L4S5QIIx")
  set(OPENOCD_TARGETCFG "target/stm32l4x.cfg")

  configure_file(
    "${NO_OS_DIR}/cmake/templates/openocd.in"
    "${CMAKE_BINARY_DIR}/openocd.cfg"
  )
endif()