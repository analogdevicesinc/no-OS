# Platform definition macro consumed by no-OS C code.
target_compile_definitions(no-os PUBLIC -DLINUX_USERSPACE_PLATFORM)
# Linux uses raw POSIX sockets; no TLS/mbedtls needed.
target_compile_definitions(no-os PUBLIC -DDISABLE_SECURE_SOCKET)

no_os_sources_ifdef(CONFIG_GPIO_LINUX_USERSPACE ${CMAKE_CURRENT_SOURCE_DIR}/linux_gpio.c)
no_os_sources_ifdef(CONFIG_I2C_LINUX_USERSPACE  ${CMAKE_CURRENT_SOURCE_DIR}/linux_i2c.c)
no_os_sources_ifdef(CONFIG_SPI_LINUX_USERSPACE  ${CMAKE_CURRENT_SOURCE_DIR}/linux_spi.c)
no_os_sources_ifdef(CONFIG_UART_LINUX_USERSPACE ${CMAKE_CURRENT_SOURCE_DIR}/linux_uart.c)
no_os_sources_ifdef(CONFIG_TIMER_LINUX_USERSPACE ${CMAKE_CURRENT_SOURCE_DIR}/linux_timer.c)

# Delay and AXI I/O helpers are always built for the linux-userspace platform.
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/linux_delay.c)
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/linux_axi_io.c)

# When IIO is enabled, linux-userspace uses POSIX TCP sockets as its network
# backend. Defining NO_OS_NETWORKING activates the networking path in the IIO
# infrastructure (iio.c, iio.h, iio_app.c) without any platform-specific
# guards needing to be added to those files.
if(${CONFIG_IIO})
    target_compile_definitions(no-os PUBLIC -DNO_OS_NETWORKING)
    target_sources(no-os PRIVATE ${NO_OS_DIR}/network/linux_socket/linux_socket.c)
    target_sources(no-os PRIVATE ${NO_OS_DIR}/network/tcp_socket.c)
endif()

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(no-os PUBLIC ${NO_OS_DIR}/network/linux_socket)
target_include_directories(no-os PUBLIC ${NO_OS_DIR}/network)
