no_os_sources_ifdef(CONFIG_SPI_GPIO_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/maxim_gpio_spi.c)
no_os_sources_ifdef(CONFIG_SYS_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/maxim_sys.c)

add_subdirectory(${MAXIM_CORE})
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/maxim_irq_common.c)
if(CONFIG_SYS_MAXIM)
    target_link_options(no-os INTERFACE -Wl,--wrap=MXC_SYS_Reset_Periph)
endif()

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_me17.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_reva1.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_me17.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C/i2c_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I2C)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA/dma_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA/dma_me17.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_ai85.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_revb.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_common.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_delay.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/pins_ai85.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/sys_ai85.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_assert.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_me17.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/LP/lp_ai85.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_me17.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC/rtc_me17.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC/rtc_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_revb.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_ai85.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_ai85.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_reva.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_common.c)

target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/RTC)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/ICC)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/FLC)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/SYS)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/TMR)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/GPIO)

target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/CMSIS/Include)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/MAX78000)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX78000/Include)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX78000/Source/heap.c)

no_os_sources_ifdef(CONFIG_CAPI_MAILBOX
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SEMA/sema_me17.c
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SEMA/sema_reva.c)
if(CONFIG_CAPI_MAILBOX)
    target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SEMA)
    target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/SEMA)
endif()

target_compile_definitions(no-os PUBLIC -DTARGET_NUM=78000)
target_compile_definitions(no-os PUBLIC -DTARGET=78000)

no_os_sources_ifdef(CONFIG_CAMERA_OV7692_MAXIM
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CAMERAIF/cameraif_ai85.c
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CAMERAIF/cameraif_reva.c
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/PT/pt_ai85.c
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/PT/pt_reva.c
    ${MAXIM_LIBRARIES}/MiscDrivers/Camera/camera.c
    ${MAXIM_LIBRARIES}/MiscDrivers/Camera/ov7692.c
    ${MAXIM_LIBRARIES}/MiscDrivers/Camera/sccb.c)
if(CONFIG_CAMERA_OV7692_MAXIM)
    target_include_directories(no-os PRIVATE
        ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CAMERAIF
        ${MAXIM_LIBRARIES}/PeriphDrivers/Source/PT)
    target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/MiscDrivers/Camera)
    target_compile_definitions(no-os PUBLIC CAMERA_OV7692)
endif()
