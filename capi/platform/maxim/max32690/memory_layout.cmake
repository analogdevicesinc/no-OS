# MAX32690 memory layout -- CAPI-LOCAL COPY.
#
# The CAPI maxim toolchain (capi/platform/maxim/toolchain.cmake) includes
# max${TARGET_NUM}/memory_layout.cmake to supply the --defsym linker symbols
# that the MAX32690 linker script references for its FLASH/SRAM region origins
# and sizes. capi/ is the replacement for drivers/ and must not depend on it,
# so the layout lives here rather than being borrowed from
# drivers/platform/maxim/max32690/. Kept in sync with that drivers copy: the
# defsym NAMES here must match the linker script references (note these differ
# from the MAX32655 set -- e.g. _FLASH_ORIGIN vs _ARM_FLASH_ORIGIN).
#
# Two variants exist:
#   - Standard (max32690.ld): vector table at 0x10000000, CM4 code follows
#   - Secure Boot / SLA (max32690_sla.ld): secure header at 0x10000000 (0x200
#     bytes), CM4 vector table and code at 0x10000200. The SLA header is
#     populated by sla_header_MAX32690.c and signed/packaged by the SBT tools
#     (sign_app, build_scp_session) when CONFIG_MAXIM_SECURE_BOOT=y.
#
# The physical memory map does not change; only the linker-region layout
# differs. See max32690_sla.ld and MSDK Tools/SBT/SBT-config.mk for the SBT
# integration.

set(_FLASH_ORIGIN       0x10000000)
set(_FLASH_SIZE         0x340000)
set(_PAL_NVM_SIZE       0x0)
set(_RISCV_FLASH_ORIGIN 0x10300000)
set(_RISCV_FLASH_SIZE   0x40000)
set(_SRAM_ORIGIN        0x20000000)
set(_SRAM_SIZE          0x100000)
set(_RISCV_SRAM_ORIGIN  0x20100000)
set(_RISCV_SRAM_SIZE    0x20000)
set(_MAILBOX_SIZE       0)

# SLA header: reserved at the start of CM4 flash for secure boot metadata.
# When CONFIG_MAXIM_SECURE_BOOT=y, the linker uses max32690_sla.ld which
# reserves [0x10000000, 0x10000200) for the .sb_sla_header section and shifts
# the actual CM4 code to 0x10000200.
set(_SLA_HEADER_SIZE    0x200)

set(MCU_LINKER_FLAGS "-Wl,--defsym,_FLASH_ORIGIN=${_FLASH_ORIGIN}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_FLASH_SIZE=${_FLASH_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_PAL_NVM_SIZE=${_PAL_NVM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_SRAM_SIZE=${_SRAM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_SRAM_ORIGIN=${_SRAM_ORIGIN}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_RISCV_FLASH_ORIGIN=${_RISCV_FLASH_ORIGIN}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_RISCV_FLASH_SIZE=${_RISCV_FLASH_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_RISCV_SRAM_ORIGIN=${_RISCV_SRAM_ORIGIN}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_RISCV_SRAM_SIZE=${_RISCV_SRAM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_MAILBOX_SIZE=${_MAILBOX_SIZE}")
