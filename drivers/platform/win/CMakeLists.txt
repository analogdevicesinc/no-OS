# Windows host-platform drivers — compiled into the no-os OBJECT library.
#
# win_delay.c is always compiled (provides no_os_udelay / no_os_mdelay via
# Windows Sleep / usleep in MinGW environments).
# win_uart.c (Windows COM port UART) is compiled only when CONFIG_UART_WIN=y.
#
# The ftd2xx library (LibMPSSE) provides SPI, I2C, GPIO and UART over a FTDI
# USB-to-MPSSE bridge and is compiled in separately via cmake/libraries/ftd2xx.cmake
# when CONFIG_FTD2XX=y — nothing extra is needed here.

target_compile_definitions(no-os PUBLIC -DWIN_PLATFORM)

target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/win_delay.c)

no_os_sources_ifdef(CONFIG_UART_WIN ${CMAKE_CURRENT_SOURCE_DIR}/win_uart.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
