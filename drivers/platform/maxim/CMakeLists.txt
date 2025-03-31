target_compile_definitions(capi_build PUBLIC -DMAXIM_PLATFORM)

no_os_sources_ifdef(CONFIG_I2C_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/max32650/maxim_i2c.c)
no_os_sources_ifdef(CONFIG_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/max32650/maxim_uart.c)
no_os_sources_ifdef(CONFIG_UART_STDIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/max32650/maxim_uart_stdio.c)
no_os_sources_ifdef(CONFIG_SPI_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/max32650/maxim_spi.c)
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/max32650/maxim_irq.c)

target_sources(capi_build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/max32650/maxim_delay.c)
target_sources(capi_build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common/maxim_dma.c)

target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/max32650)
target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

add_subdirectory(max32650)

target_compile_definitions(capi_build PUBLIC -DMXC_SPI_VERSION=v1)
target_compile_definitions(capi_build PUBLIC -DTARGET=32650)
target_compile_definitions(capi_build PUBLIC -DTARGET_REV=0x4131)

set(project_elf ${CMAKE_BINARY_DIR}/projects/iio_demo/iio_demo)

add_custom_target(gdb_server
        COMMAND ${OPENOCD_PATH}/bin/openocd -s ${OPENOCD_PATH}/scripts
                                        -f interface/cmsis-dap.cfg
                                        -f /target/max32650.cfg
                                        -c "init" &
        DEPENDS capi_build
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
)
add_custom_target(flash
        COMMAND ${OPENOCD_PATH}/bin/openocd -s ${OPENOCD_PATH}/scripts
                                        -f interface/cmsis-dap.cfg
                                        -f target/max32650.cfg
                                        -c "program ${project_elf} verify reset exit"
        DEPENDS capi_build
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
)