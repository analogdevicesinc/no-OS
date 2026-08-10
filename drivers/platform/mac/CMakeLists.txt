# macOS host-platform drivers — compiled into the no-os OBJECT library.
#
# mac_delay.c is always compiled (provides no_os_udelay / no_os_mdelay via
# POSIX usleep).
# mac_uart.c (macOS serial-port UART via /dev/tty.*) is compiled only when
# CONFIG_UART_MAC=y.
#
# The ftd2xx library (LibMPSSE) provides SPI, I2C, GPIO and UART over a FTDI
# USB-to-MPSSE bridge and is compiled in separately via cmake/libraries/ftd2xx.cmake
# when CONFIG_FTD2XX=y — nothing extra is needed here.

target_compile_definitions(no-os PUBLIC -DMAC_PLATFORM)

target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mac_delay.c)

no_os_sources_ifdef(CONFIG_UART_MAC ${CMAKE_CURRENT_SOURCE_DIR}/mac_uart.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
