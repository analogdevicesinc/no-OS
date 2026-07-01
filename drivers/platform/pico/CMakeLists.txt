# Platform definitions
target_compile_definitions(no-os PUBLIC -DPICO_PLATFORM)

# no-OS pico platform drivers (one source per Kconfig symbol).
no_os_sources_ifdef(CONFIG_GPIO_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_gpio.c)
no_os_sources_ifdef(CONFIG_GPIO_IRQ_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_gpio_irq.c)
no_os_sources_ifdef(CONFIG_I2C_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_i2c.c)
no_os_sources_ifdef(CONFIG_SPI_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_spi.c)
no_os_sources_ifdef(CONFIG_UART_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_uart.c)
no_os_sources_ifdef(CONFIG_IRQ_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_irq.c)
no_os_sources_ifdef(CONFIG_TIMER_PICO ${CMAKE_CURRENT_SOURCE_DIR}/pico_timer.c)

# Delay helper is always built for the pico platform.
target_sources(no-os PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pico_delay.c)

target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Generated SDK headers (pico/version.h, pico/config_autogen.h) and linker
# fragment (pico_flash_region.ld) are produced by config_pico_sdk() into these
# directories. They must be on the include path here because this file runs
# before the project's CMakeLists (which calls config_pico_sdk()).
target_include_directories(no-os PUBLIC ${CMAKE_BINARY_DIR}/build/generated/pico_base)

# ---------------------------------------------------------------------------
# pico-sdk sources (ported from tools/scripts/pico.mk PLATFORM_SRCS).
# ---------------------------------------------------------------------------
set(PICO_SDK_SRCS
    src/common/hardware_claim/claim.c
    src/common/pico_sync/critical_section.c
    src/common/pico_sync/lock_core.c
    src/common/pico_sync/mutex.c
    src/common/pico_sync/sem.c
    src/common/pico_time/time.c
    src/common/pico_time/timeout_helper.c
    src/common/pico_util/datetime.c
    src/common/pico_util/pheap.c
    src/common/pico_util/queue.c
    src/rp2_common/hardware_clocks/clocks.c
    src/rp2_common/hardware_gpio/gpio.c
    src/rp2_common/hardware_i2c/i2c.c
    src/rp2_common/hardware_irq/irq.c
    src/rp2_common/hardware_pll/pll.c
    src/rp2_common/hardware_spi/spi.c
    src/rp2_common/hardware_sync/sync.c
    src/rp2_common/hardware_sync_spin_lock/sync_spin_lock.c
    src/rp2_common/hardware_ticks/ticks.c
    src/rp2_common/hardware_timer/timer.c
    src/rp2_common/hardware_uart/uart.c
    src/rp2_common/hardware_vreg/vreg.c
    src/rp2_common/hardware_watchdog/watchdog.c
    src/rp2_common/hardware_xosc/xosc.c
    src/rp2_common/pico_atomic/atomic.c
    src/rp2_common/pico_bootrom/bootrom.c
    src/rp2_common/pico_clib_interface/newlib_interface.c
    src/rp2_common/pico_double/double_init_rom_rp2040.c
    src/rp2_common/pico_double/double_math.c
    src/rp2_common/pico_float/float_init_rom_rp2040.c
    src/rp2_common/pico_float/float_math.c
    src/rp2_common/pico_malloc/malloc.c
    src/rp2_common/pico_mem_ops/mem_ops.c
    src/rp2_common/pico_platform_common/common.c
    src/rp2_common/pico_platform_panic/panic.c
    src/rp2_common/pico_printf/printf.c
    src/rp2_common/pico_runtime/runtime.c
    src/rp2_common/pico_runtime_init/runtime_init.c
    src/rp2_common/pico_runtime_init/runtime_init_clocks.c
    src/rp2_common/pico_runtime_init/runtime_init_stack_guard.c
    src/rp2_common/pico_standard_binary_info/standard_binary_info.c
    src/rp2_common/pico_stdio/stdio.c
    src/rp2_common/pico_stdio_uart/stdio_uart.c
    src/rp2_common/pico_stdlib/stdlib.c
    src/rp2040/pico_platform/platform.c
)
foreach(_src ${PICO_SDK_SRCS})
    target_sources(no-os PRIVATE ${PICO_SDK_PATH}/${_src})
endforeach()

# pico-sdk assembly sources (ported from pico.mk ASM_SRCS).
set(PICO_SDK_ASM_SRCS
    src/rp2_common/hardware_irq/irq_handler_chain.S
    src/rp2_common/pico_bit_ops/bit_ops_aeabi.S
    src/rp2_common/pico_divider/divider_hardware.S
    src/rp2_common/pico_crt0/crt0.S
    src/rp2_common/pico_double/double_aeabi_rp2040.S
    src/rp2_common/pico_double/double_v1_rom_shim_rp2040.S
    src/rp2_common/pico_float/float_aeabi_rp2040.S
    src/rp2_common/pico_float/float_v1_rom_shim_rp2040.S
    src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S
    src/rp2_common/pico_mem_ops/mem_ops_aeabi.S
)
foreach(_asm ${PICO_SDK_ASM_SRCS})
    target_sources(no-os PRIVATE ${PICO_SDK_PATH}/${_asm})
endforeach()

# ---------------------------------------------------------------------------
# pico-sdk include directories (ported from pico.mk PLATFORM_INCS_PATH).
# ---------------------------------------------------------------------------
set(PICO_SDK_INCS
    src/boards/include
    src/rp2_common/boot_bootrom_headers/include
    src/common/boot_picobin_headers/include
    src/common/boot_picoboot_headers/include
    src/common/boot_uf2_headers/include
    src/rp2_common/cmsis/include
    src/rp2_common/cmsis/stub/CMSIS/Core/Include
    src/rp2_common/cmsis/stub/CMSIS/Device/RP2040/Include
    src/rp2_common/hardware_base/include
    src/rp2_common/hardware_boot_lock/include
    src/rp2_common/hardware_clocks/include
    src/rp2_common/hardware_divider/include
    src/rp2_common/hardware_dma/include
    src/rp2_common/hardware_exception/include
    src/rp2_common/hardware_gpio/include
    src/rp2_common/hardware_i2c/include
    src/rp2_common/hardware_irq/include
    src/rp2_common/hardware_pll/include
    src/rp2_common/hardware_resets/include
    src/rp2_common/hardware_spi/include
    src/rp2_common/hardware_sync/include
    src/rp2_common/hardware_sync_spin_lock/include
    src/rp2_common/hardware_ticks/include
    src/rp2_common/hardware_timer/include
    src/rp2_common/hardware_uart/include
    src/rp2_common/hardware_vreg/include
    src/rp2_common/hardware_watchdog/include
    src/rp2_common/hardware_xosc/include
    src/rp2_common/pico_atomic/include
    src/rp2_common/pico_bootrom/include
    src/rp2_common/pico_clib_interface/include
    src/rp2_common/pico_double/include
    src/rp2_common/pico_flash/include
    src/rp2_common/pico_float/include
    src/rp2_common/pico_int64_ops/include
    src/rp2_common/pico_malloc/include
    src/rp2_common/pico_mem_ops/include
    src/rp2_common/pico_platform_common/include
    src/rp2_common/pico_platform_compiler/include
    src/rp2_common/pico_platform_panic/include
    src/rp2_common/pico_platform_sections/include
    src/rp2_common/pico_printf/include
    src/rp2_common/pico_runtime/include
    src/rp2_common/pico_runtime_init/include
    src/rp2_common/pico_stdio/include
    src/rp2_common/pico_stdio_uart/include
    src/rp2_common/pico_time_adapter/include
    src/rp2040/hardware_regs/include
    src/rp2040/hardware_structs/include
    src/rp2040/pico_platform/include
    src/common/pico_base_headers/include
    src/common/pico_binary_info/include
    src/common/pico_bit_ops_headers/include
    src/common/pico_divider_headers/include
    src/common/pico_stdlib_headers/include
    src/common/pico_sync/include
    src/common/pico_time/include
    src/common/pico_util/include
    src/common/hardware_claim/include
)
foreach(_inc ${PICO_SDK_INCS})
    target_include_directories(no-os PUBLIC ${PICO_SDK_PATH}/${_inc})
endforeach()

# ---------------------------------------------------------------------------
# Defined symbols (ported from pico.mk DEFINES). These select the pico-sdk
# library implementations and board, and must reach both no-os and the SDK
# sources, hence PUBLIC.
# ---------------------------------------------------------------------------
target_compile_definitions(no-os PUBLIC
    LIB_PICO_ATOMIC=1
    LIB_PICO_BIT_OPS=1
    LIB_PICO_BIT_OPS_PICO=1
    LIB_PICO_CLIB_INTERFACE=1
    LIB_PICO_CRT0=1
    LIB_PICO_DIVIDER=1
    LIB_PICO_DIVIDER_HARDWARE=1
    LIB_PICO_DOUBLE=1
    LIB_PICO_DOUBLE_PICO=1
    LIB_PICO_FLOAT=1
    LIB_PICO_FLOAT_PICO=1
    LIB_PICO_INT64_OPS=1
    LIB_PICO_INT64_OPS_PICO=1
    LIB_PICO_MALLOC=1
    LIB_PICO_MEM_OPS=1
    LIB_PICO_MEM_OPS_PICO=1
    LIB_PICO_PLATFORM=1
    LIB_PICO_PRINTF=1
    LIB_PICO_PRINTF_PICO=1
    LIB_PICO_RUNTIME=1
    LIB_PICO_RUNTIME_INIT=1
    LIB_PICO_STANDARD_BINARY_INFO=1
    LIB_PICO_STANDARD_LINK=1
    LIB_PICO_STDIO=1
    LIB_PICO_STDIO_UART=1
    LIB_PICO_STDLIB=1
    LIB_PICO_SYNC=1
    LIB_PICO_SYNC_CRITICAL_SECTION=1
    LIB_PICO_SYNC_MUTEX=1
    LIB_PICO_SYNC_SEM=1
    LIB_PICO_TIME=1
    LIB_PICO_TIME_ADAPTER=1
    LIB_PICO_UTIL=1
    PICO_32BIT=1
    PICO_BOARD="pico"
    PICO_BUILD=1
    PICO_CMAKE_BUILD_TYPE="Debug"
    PICO_COPY_TO_RAM=0
    PICO_CXX_ENABLE_EXCEPTIONS=0
    PICO_NO_FLASH=0
    PICO_NO_HARDWARE=0
    PICO_ON_DEVICE=1
    PICO_RP2040=1
    PICO_USE_BLOCKED_RAM=0
    PICO_HEAP_SIZE=0x1000
    CFG_TUSB_MCU=OPT_MCU_RP2040
    PICO_DEFAULT_IRQ_PRIORITY=0
)
