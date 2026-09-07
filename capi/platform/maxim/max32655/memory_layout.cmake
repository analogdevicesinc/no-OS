# MAX32655 memory layout -- CAPI-LOCAL COPY.
#
# The CAPI maxim toolchain (capi/platform/maxim/toolchain.cmake) includes
# max${TARGET_NUM}/memory_layout.cmake to supply the --defsym linker symbols
# that MAX32655's linker script (max32655.ld) dereferences for its FLASH/SRAM
# region origins and sizes. capi/ is the replacement for drivers/ and must not
# depend on it, so the layout lives here rather than being borrowed from
# drivers/platform/maxim/max32655/. Identical to the drivers copy for now; add
# one of these per target as it is brought up (MAX32657 first, then widen).

set(_FLASH_ORIGIN       0x10000000)
set(_FLASH_SIZE         0x80000)
set(_ARM_FLASH_SIZE     0x80000)
set(_PAL_NVM_SIZE       0x0)
set(_RISCV_FLASH_ORIGIN 0x10300000)
set(_RISCV_FLASH_SIZE   0x00000)
set(_SRAM_ORIGIN        0x20000000)
set(_SRAM_SIZE      0x1C000)
set(_ARM_SRAM_SIZE      0x1C000)
set(_RISCV_SRAM_SIZE    0x0000)
set(_MAILBOX_SIZE       0)

set(MCU_LINKER_FLAGS "-Wl,--defsym,_ARM_FLASH_ORIGIN=${_FLASH_ORIGIN}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_ARM_FLASH_SIZE=${_ARM_FLASH_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_FLASH_SIZE=${_FLASH_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_PAL_NVM_SIZE=${_PAL_NVM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_SRAM_SIZE=${_SRAM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_ARM_SRAM_SIZE=${_ARM_SRAM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_ARM_SRAM_ORIGIN=${_SRAM_ORIGIN}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,RISCV_FLASH_SIZE=${_RISCV_FLASH_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,RISCV_SRAM_SIZE=${_RISCV_SRAM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_MAILBOX_SIZE=${_MAILBOX_SIZE}")
