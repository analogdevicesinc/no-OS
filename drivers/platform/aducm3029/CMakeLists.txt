# ADuCM3029 platform driver sources
target_compile_definitions(no-os PUBLIC -DADUCM_PLATFORM)

# Heap size required by the RTE startup.
target_compile_definitions(no-os PUBLIC -D__HEAP_SIZE=15000)

# no-OS platform driver sources, gated by Kconfig.
no_os_sources_ifdef(CONFIG_UART_ADUCM3029       ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_uart.c)
no_os_sources_ifdef(CONFIG_UART_STDIO_ADUCM3029 ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_uart_stdio.c)
no_os_sources_ifdef(CONFIG_IRQ_ADUCM3029        ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_irq.c)
no_os_sources_ifdef(CONFIG_GPIO_ADUCM3029       ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_gpio.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_ADUCM3029   ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_gpio_irq.c)
no_os_sources_ifdef(CONFIG_I2C_ADUCM3029        ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_i2c.c)
no_os_sources_ifdef(CONFIG_SPI_ADUCM3029        ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_spi.c)
no_os_sources_ifdef(CONFIG_TIMER_ADUCM3029      ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_timer.c)
no_os_sources_ifdef(CONFIG_RTC_ADUCM3029        ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_rtc.c)
no_os_sources_ifdef(CONFIG_PWM_ADUCM3029        ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_pwm.c)
no_os_sources_ifdef(CONFIG_ADC_ADUCM3029        ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_adc.c)
no_os_sources_ifdef(CONFIG_IIO_ADUCM3029        ${CMAKE_CURRENT_SOURCE_DIR}/iio_aducm3029.c)
no_os_sources_ifdef(CONFIG_FLASH_ADUCM3029      ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_flash.c)
no_os_sources_ifdef(CONFIG_TRNG_ADUCM3029       ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_trng.c)

# Platform init is always required.
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/platform_init.c)

# Delay (always included when not using FreeRTOS, matches maxim).
if(NOT ${CONFIG_FREERTOS})
    target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/aducm3029_delay.c)
endif()

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Device Family Pack + CMSIS Core includes.
target_include_directories(no-os PUBLIC ${ADUCM_DFP}/Include)
target_include_directories(no-os PUBLIC ${ADUCM_CMSIS_CORE}/Include)

# DFP driver sources. The full set is compiled (matches the legacy build which
# wildcards $(DFP_DRIVERS)/*.c); two data files are excluded because they are
# not used on the ADuCM3029.
file(GLOB_RECURSE _dfp_sources "${ADUCM_DFP}/Source/drivers/*.c")
list(REMOVE_ITEM _dfp_sources
    "${ADUCM_DFP}/Source/drivers/flash/adi_flash_data.c"
    "${ADUCM_DFP}/Source/drivers/rtc/adi_rtc_data.c"
)
target_sources(no-os PRIVATE ${_dfp_sources})
