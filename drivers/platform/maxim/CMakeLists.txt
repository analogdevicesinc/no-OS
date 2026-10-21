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
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_usb_uart.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/core/musbhsfc/usb.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/core/usb_event.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/dbg_log/dbg_log.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/ccid.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/fifo.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/cdc_acm.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/enumerate/enumerate.c)

# CAPI driver sources
no_os_sources_ifdef(CONFIG_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart.c)
no_os_sources_ifdef(CONFIG_UART_STDIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart_stdio.c)
no_os_sources_ifdef(CONFIG_GPIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_gpio.c)
no_os_sources_ifdef(CONFIG_TIMER_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_timer.c)
no_os_sources_ifdef(CONFIG_I2C_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_i2c.c)
no_os_sources_ifdef(CONFIG_SPI_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_spi.c)

no_os_sources_ifdef(CONFIG_DMA_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/common/maxim_dma.c)

if (NOT CONFIG_CAPI)
	target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_init.c)

	# Delay (always included when not using FreeRTOS)
	if(NOT ${CONFIG_FREERTOS})
        	target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_delay.c)
	endif()
endif()

no_os_include_dir_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/include/core)
no_os_include_dir_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/include/core/musbhsfc)
no_os_include_dir_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/include/dbg_log)
no_os_include_dir_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/include/enumerate)
no_os_include_dir_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/include/devclass)

# libiio (CONFIG_LIBIIO): the part's own ADC as a libiio device, and the
# MAXUSB glue behind libiio's USB transport.
if(CONFIG_IIO_ADC_MAXIM)
	target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common/maxim_iio_adc.c)
	target_compile_definitions(no-os PRIVATE IIO_ADC_NAME="${TARGET}-adc")
	if(TARGET_NUM STREQUAL "32690")
		target_compile_definitions(no-os PRIVATE IIO_ADC_CLOCK=MXC_ADC_CLK_IBRO)
	elseif(TARGET_NUM STREQUAL "32655")
		target_compile_definitions(no-os PRIVATE IIO_ADC_REF_VOLTAGE_MV=1220)
	else()
		message(FATAL_ERROR "IIO_ADC_MAXIM: not brought up on ${TARGET} yet")
	endif()
endif()
no_os_sources_ifdef(CONFIG_LIBIIO_TRANSPORT_USB ${CMAKE_CURRENT_SOURCE_DIR}/common/maxim_iio_usb.c)
# libiio's responder needs far more than the startup file's 4 KiB stack.
if(CONFIG_LIBIIO)
	target_compile_definitions(no-os PRIVATE __STACK_SIZE=0x10000)
endif()

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET})
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

# Platform-specific compile definitions
target_compile_definitions(no-os PUBLIC -DMXC_SPI_VERSION=v1)
target_compile_definitions(no-os PUBLIC -DTARGET_REV=0x4131)
