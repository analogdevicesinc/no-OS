# MAX32665/66 memory layout -- CAPI-LOCAL COPY.
#
# The CAPI maxim toolchain (capi/platform/maxim/toolchain.cmake) includes
# max${TARGET_NUM}/memory_layout.cmake to supply the --defsym linker symbols the
# part's linker script dereferences. capi/ is the replacement for drivers/ and
# must not depend on it, so the layout lives here; there is no drivers copy of
# this one to mirror.
#
# Unlike MAX32655/MAX32690, max32665.ld hardcodes the region ORIGINs and derives
# nearly everything itself. It needs exactly two symbols from outside, and the
# NAMES differ from the other parts -- SRAM_SIZE here has no leading underscore
# while _PAL_NVM_SIZE does. Do not copy another part's defsym list.
#
#   SRAM_SIZE      the .ld comments "SRAM_SIZE must be defined"; it splits the
#                  total between OTP/shared/SDMA and the leftover SRAM region:
#                      SRAM_LEN = SRAM_SIZE - OTP_LEN - SHARED_LEN - SDMA_LEN
#   _PAL_NVM_SIZE  flash reserved at the top for the BLE PAL non-volatile DB:
#                      FLASH_MEM_SIZE = 0x100000 - _PAL_NVM_SIZE
#                  Zero for non-BLE builds, which is what CAPI selftest is.
#
# SDMA_CODE_SIZE / SDMA_DATA_SIZE are optional -- the script wraps them in
# DEFINED() and falls back to 0, which also zeroes OTP_LEN and SHARED_LEN. The
# SDMA (smart DMA) core is unused by CAPI, so they are deliberately left unset
# rather than defined to 0: an explicit 0 would still take the "SDMA present"
# branch for OTP_LEN and SHARED_LEN.
#
# Values from MXC_SRAM_MEM_SIZE in MSDK max32665.h (0x8C000 = 560 KiB). Flash is
# 1 MiB and is hardcoded in the .ld, so no flash symbol is passed.
#
# STATUS: builds only -- no MAX32665/66 board has been run.

set(_SRAM_SIZE          0x8C000)
set(_PAL_NVM_SIZE       0x0)

set(MCU_LINKER_FLAGS "-Wl,--defsym,SRAM_SIZE=${_SRAM_SIZE}")
set(MCU_LINKER_FLAGS "${MCU_LINKER_FLAGS} -Wl,--defsym,_PAL_NVM_SIZE=${_PAL_NVM_SIZE}")
