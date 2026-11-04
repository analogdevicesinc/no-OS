target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(no-os PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/maxim_init.c
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/mxc_lock.c
    ${MAXIM_LIBRARIES}/PeriphDrivers/Source/SYS/nvic_table.c
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/GCC/startup_max32690.S
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/system_max32690.c)
if(NOT CONFIG_FREERTOS)
    target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/maxim_delay.c)
endif()
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/maxim_irq.c)
