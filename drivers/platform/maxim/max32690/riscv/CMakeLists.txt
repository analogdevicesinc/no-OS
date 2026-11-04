if(CONFIG_FREERTOS)
    message(FATAL_ERROR "MAX32690 RISC-V FreeRTOS is not supported by this platform")
endif()
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(no-os PUBLIC RV32 RISCV
    MAXIM_RISCV_TIME_TIMER=${MAXIM_RISCV_TIME_TIMER})
# Memory symbols exported by startup_riscv_max32690.S
target_compile_definitions(no-os PRIVATE
    RISCV_FLASH_ORIGIN=${MAXIM_RISCV_FLASH_ORIGIN}
    RISCV_FLASH_SIZE=${MAXIM_RISCV_FLASH_LENGTH}
    RISCV_SRAM_ORIGIN=${_RISCV_SRAM_ORIGIN}
    RISCV_SRAM_SIZE=${_RISCV_SRAM_SIZE})
target_sources(no-os PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/maxim_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maxim_delay.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maxim_lock.c
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/GCC/startup_riscv_max32690.S
    ${MAXIM_LIBRARIES}/CMSIS/Device/Maxim/MAX32690/Source/system_riscv_max32690.c)
no_os_sources_ifdef(CONFIG_IRQ_MAXIM ${CMAKE_CURRENT_SOURCE_DIR}/maxim_irq.c)
target_link_libraries(no-os INTERFACE c_nano m gcc nosys)
