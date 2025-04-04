list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(MaximSDK REQUIRED)

message(STATUS "MaximSDK = ${MaximSDK_FOUND}")

if (${MaximSDK_FOUND})
    message(STATUS "MaximSDK found")
else()
    message(FATAL_ERROR "MaximSDK not found!")
endif()

target_compile_definitions(capi_build PUBLIC -DMAXIM_PLATFORM)

add_subdirectory(${TARGET})

no_os_sources_ifdef(CONFIG_I2C_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_i2c.c)
no_os_sources_ifdef(CONFIG_UART_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart.c)
no_os_sources_ifdef(CONFIG_UART_STDIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_uart_stdio.c)
no_os_sources_ifdef(CONFIG_SPI_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_spi.c)
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_irq.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_gpio_irq.c)
no_os_sources_ifdef(CONFIG_GPIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_gpio.c)

target_sources(capi_build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}/maxim_delay.c)
target_sources(capi_build PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/common/maxim_dma.c)

target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET})
target_include_directories(capi_build PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

target_compile_definitions(capi_build PUBLIC -DMXC_SPI_VERSION=v1)
target_compile_definitions(capi_build PUBLIC -DTARGET_REV=0x4131)

set(project_elf ${CMAKE_BINARY_DIR}/projects/eval-adxl355-pmdz/${TARGET}.elf)

add_custom_target(gdb_server
        COMMAND ${OPENOCD_PATH}/bin/openocd -s ${OPENOCD_PATH}/scripts
                                        -f interface/cmsis-dap.cfg
                                        -f /target/$[TARGET].cfg
                                        -c "init" &
        DEPENDS capi_build
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
)

add_custom_target(flash
        COMMAND ${OPENOCD_PATH}/bin/openocd -s ${OPENOCD_PATH}/scripts
                                        -f interface/cmsis-dap.cfg
                                        -f target/${TARGET}.cfg
                                        -c "program ${project_elf} verify reset exit"
        DEPENDS capi_build
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        USES_TERMINAL
)