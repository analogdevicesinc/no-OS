# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
# find_package(MaximSDK REQUIRED)

# message(STATUS "MaximSDK = ${MaximSDK_FOUND}")

# if (${MaximSDK_FOUND})
#     message(STATUS "MaximSDK found")
# else()
#     message(FATAL_ERROR "MaximSDK not found!")
# endif()

target_compile_definitions(no-os PUBLIC -DMAXIM_PLATFORM)

add_subdirectory(${TARGET})

no_os_sources_ifdef(CONFIG_I2C_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_i2c.c)
no_os_sources_ifdef(CONFIG_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart.c)
no_os_sources_ifdef(CONFIG_UART_STDIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart_stdio.c)
no_os_sources_ifdef(CONFIG_SPI_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_spi.c)
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_irq.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_gpio_irq.c)
no_os_sources_ifdef(CONFIG_GPIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_gpio.c)

no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_usb_uart.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/core/musbhsfc/usb.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/core/usb_event.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/dbg_log/dbg_log.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/ccid.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/fifo.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/devclass/cdc_acm.c)
no_os_sources_ifdef(CONFIG_USB_UART_MAXIM ${MAXIM_LIBRARIES}/MAXUSB/src/enumerate/enumerate.c)

target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/core)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/core/musbhsfc)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/dbg_log)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/enumerate)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MAXUSB/include/devclass)

if(${CONFIG_USB_UART_MAXIM})
        target_compile_definitions(no-os PUBLIC -DNO_OS_USB_UART=1)
endif()

if(NOT ${CONFIG_FREERTOS})
        target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_delay.c)
endif()

target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common/maxim_dma.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET})
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

target_compile_definitions(no-os PUBLIC -DMXC_SPI_VERSION=v1)
target_compile_definitions(no-os PUBLIC -DTARGET_REV=0x4131)

configure_file(
    "${NO_OS_DIR}/cmake/templates/openocd.in"
    "${CMAKE_BINARY_DIR}/openocd.cfg"
  )

# if(OPENOCD_PATH)
#         set(OPENOCD_INTERFACE "interface/cmsis-dap.cfg")
#         set(OPENOCD_CHIPNAME "max32690")
#         set(OPENOCD_TARGETCFG "target/max32690.cfg")

#         configure_file(
#         "${NO_OS_DIR}/cmake/templates/openocd.in"
#         "${CMAKE_BINARY_DIR}/openocd.cfg"
#         )
# endif()

# set(project_elf ${CMAKE_BINARY_DIR}/projects/eval-adxl355-pmdz/${TARGET}.elf)

# add_custom_target(gdb_server
#         COMMAND ${OPENOCD_PATH}/bin/openocd -s ${OPENOCD_PATH}/scripts
#                                         -f interface/cmsis-dap.cfg
#                                         -f /target/$[TARGET].cfg
#                                         -c "init" &
#         DEPENDS no-os
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#         USES_TERMINAL
# )

# add_custom_target(flash
#         COMMAND ${OPENOCD_PATH}/bin/openocd -s ${OPENOCD_PATH}/scripts
#                                         -f interface/cmsis-dap.cfg
#                                         -f target/${TARGET}.cfg
#                                         -c "program ${project_elf} verify reset exit"
#         DEPENDS no-os
#         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#         USES_TERMINAL
# )