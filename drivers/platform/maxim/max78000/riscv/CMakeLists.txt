if(CONFIG_FREERTOS)
    message(FATAL_ERROR "MAX78000 RISC-V FreeRTOS is not supported by this platform")
endif()
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(no-os PUBLIC RV32 RISCV
    MAXIM_RISCV_TIME_TIMER=${MAXIM_RISCV_TIME_TIMER})
target_sources(no-os PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/maxim_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maxim_delay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maxim_lock.c
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX78000/Source/GCC/startup_riscv_max78000.S
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX78000/Source/system_riscv_max78000.c)
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/maxim_irq.c)
target_link_libraries(no-os INTERFACE c_nano m gcc nosys)
set_property(TARGET no-os APPEND PROPERTY INTERFACE_LINK_DEPENDS
    "${CMAKE_BINARY_DIR}/buildrv/common_riscv.ld"
    "${CMAKE_BINARY_DIR}/common.ld")
