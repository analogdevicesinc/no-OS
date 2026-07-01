# Platform definitions. PICO_PLATFORM is a no-OS macro (used in no-OS C code for
# platform selection, e.g. iio/iio_app and projects' platform_includes.h). It is
# unrelated to the pico-sdk CMake variable of the same name.
target_compile_definitions(no-os PUBLIC -DPICO_PLATFORM)

# no-OS pico platform drivers (one source per Kconfig symbol).
no_os_sources_ifdef(CONFIG_GPIO_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_gpio.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_gpio_irq.c)
no_os_sources_ifdef(CONFIG_I2C_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_i2c.c)
no_os_sources_ifdef(CONFIG_SPI_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_spi.c)
no_os_sources_ifdef(CONFIG_UART_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_uart.c)
no_os_sources_ifdef(CONFIG_IRQ_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_irq.c)
no_os_sources_ifdef(CONFIG_TIMER_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_timer.c)

# Delay helper is always built for the pico platform.
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pico_delay.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ---------------------------------------------------------------------------
# pico-sdk headers for the no-OS pico drivers above. no-os is an OBJECT library,
# so linking a *source-bearing* pico-sdk INTERFACE lib here would compile that
# lib's sources into no-os AND, via propagation, into the executable -> duplicate
# symbols at link. We therefore give no-os only the header-only (*_headers)
# targets, enough for the no-OS pico drivers to compile; the full source-bearing
# SDK libraries are linked once, on the executable, in config_pico_sdk()
# (cmake/pico/pico_platform_sdk.cmake). The SDK's own CMake (via pico_sdk_init())
# still supplies all sources, include dirs, LIB_PICO_* defines, boot2, the linker
# script and the libc/soft-float --wrap options transitively -- replacing the
# hand-maintained PICO_SDK_SRCS / PICO_SDK_INCS / LIB_PICO_* lists.
target_link_libraries(no-os PUBLIC
    pico_stdlib_headers
    hardware_gpio_headers
    hardware_i2c_headers
    hardware_irq_headers
    hardware_resets_headers
    hardware_spi_headers
    hardware_timer_headers
    hardware_uart_headers
    pico_stdio_uart_headers
    pico_time_headers
)

# Policy defines the SDK does not set from a default init.
target_compile_definitions(no-os PUBLIC
    PICO_HEAP_SIZE=0x1000
    PICO_DEFAULT_IRQ_PRIORITY=0
)
