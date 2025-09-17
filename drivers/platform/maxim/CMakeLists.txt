# Platform definitions
target_compile_definitions(no-os PUBLIC -DMAXIM_PLATFORM)

# Add target-specific subdirectory
add_subdirectory(${TARGET})

no_os_sources_ifdef(CONFIG_GPIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_gpio.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_gpio_irq.c)
no_os_sources_ifdef(CONFIG_I2C_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_i2c.c)
no_os_sources_ifdef(CONFIG_SPI_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_spi.c)
no_os_sources_ifdef(CONFIG_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart.c)
no_os_sources_ifdef(CONFIG_UART_STDIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart_stdio.c)
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_irq.c)
no_os_sources_ifdef(CONFIG_PWM_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_pwm.c)
no_os_sources_ifdef(CONFIG_TIMER_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_timer.c)
no_os_sources_ifdef(CONFIG_RTC_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_rtc.c)
no_os_sources_ifdef(CONFIG_TRNG_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_trng.c)
no_os_sources_ifdef(CONFIG_INIT_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_init.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_usb_uart.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/core/musbhsfc/usb.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/core/usb_event.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/dbg_log/dbg_log.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/ccid.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/fifo.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/cdc_acm.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/enumerate/enumerate.c)

no_os_sources_ifdef(CONFIG_DMA_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/common/maxim_dma.c)

# Delay (always included when not using FreeRTOS)
if(NOT ${CONFIG_FREERTOS})
        target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_delay.c)
endif()

# USB UART library include directories
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/core)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/core/musbhsfc)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/dbg_log)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/enumerate)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/devclass)

# USB UART compile definitions
if(${CONFIG_USB_UART_MAXIM})
        target_compile_definitions(no-os PUBLIC -DNO_OS_USB_UART=1)
endif()

# Include directories
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET})
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

# Platform-specific compile definitions
target_compile_definitions(no-os PUBLIC -DMXC_SPI_VERSION=v1)
target_compile_definitions(no-os PUBLIC -DTARGET_REV=0x4131)

# OpenOCD configuration
configure_file(
    "${NO_OS_DIR}/cmake/templates/openocd.in"
    "${CMAKE_BINARY_DIR}/openocd.cfg"
  )