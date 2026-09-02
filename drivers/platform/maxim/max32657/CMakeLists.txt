target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_delay.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/pins_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/sys_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/nvic_table.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_lock.c)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/AES/aes_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/AES/aes_revb.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/AES)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CRC/crc_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CRC/crc_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/CRC)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA/dma_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA/dma_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/DMA)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC/flc_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/FLC)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO/gpio_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/GPIO)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I3C/i3c_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I3C/i3c_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/I3C)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC/icc_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/ICC)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/LP/lp_me30.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/LP)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC/rtc_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC/rtc_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/RTC)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI/spi_reva1.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SPI)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR/tmr_revb.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TMR)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TRNG/trng_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TRNG/trng_revb.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/TRNG)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART/uart_revb.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/UART)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WDT/wdt_common.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WDT/wdt_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WDT/wdt_revb.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WDT)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WUT/wut_me30.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WUT/wut_reva.c)
target_include_directories(no-os PRIVATE ${MAXIM_LIBRARIES}/PeriphDrivers/Source/WUT)

# target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/CMSIS/Include)
if (CFS)
	target_include_directories(no-os PUBLIC ${CFS}/SDK/zephyr/modules/hal/cmsis_6/CMSIS/Core/Include)
	target_include_directories(no-os PUBLIC ${CFS}/SDK/zephyr/modules/hal/cmsis_6/CMSIS/Core/Include/m-profile)
else()
	target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/CMSIS/5.9.0/Core/Include)
endif()

target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/PeriphDrivers/Include/MAX32657)
target_include_directories(no-os PUBLIC ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32657/Include)

target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32657/Source/GCC/startup_max32657.S)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32657/Source/heap.c)
target_sources(no-os PRIVATE ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32657/Source/system_max32657.c)
