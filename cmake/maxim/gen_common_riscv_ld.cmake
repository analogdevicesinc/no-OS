# Derive the RISC-V flash origin for a Maxim dual-core build.
#
# Run at build time via `cmake -P`. Reads the ARM premap linker map, finds the
# flash address of the `_riscv_boot` symbol (the start of the reserved
# .riscv_flash region), and writes the definitions the stock
# max<N>_riscv.ld pulls in through its `INCLUDE "buildrv/common_riscv.ld"`:
#
#   __FlashStart  = <address of _riscv_boot>;
#   __FlashLength = 0x10080000 - __FlashStart;   /* to the top of flash */
#
# So the RISC-V image is linked to run exactly where the ARM link placed it.
#
# Required -D arguments:
#   MAP_FILE  absolute path to the ARM premap .map
#   OUT_FILE  absolute path to write common_riscv.ld

if(NOT DEFINED MAP_FILE OR NOT DEFINED OUT_FILE)
    message(FATAL_ERROR "gen_common_riscv_ld: MAP_FILE and OUT_FILE are required")
endif()

if(NOT EXISTS "${MAP_FILE}")
    message(FATAL_ERROR "gen_common_riscv_ld: map file not found: ${MAP_FILE}")
endif()

file(READ "${MAP_FILE}" _map)

# GNU ld prints the symbol assignment as e.g.
#   0x0000000010012400                _riscv_boot = .
string(REGEX MATCH "0x0*([0-9a-fA-F]+)[ \t]+_riscv_boot" _match "${_map}")

if(NOT _match)
    message(FATAL_ERROR
        "gen_common_riscv_ld: could not find _riscv_boot address in ${MAP_FILE}")
endif()

set(_addr "0x${CMAKE_MATCH_1}")

# MAX32690 RISC-V SRAM region: 0x20100000-0x20120000 (128 KB)
set(_sram_origin "0x20100000")
set(_sram_size "0x20000")
set(_mailbox_size "0x100")
set(_pal_nvm_size "0x0")

file(WRITE "${OUT_FILE}"
    "/* Auto-generated -- do not edit. RISC-V flash and SRAM config from ARM map. */\n"
    "_RISCV_FLASH_ORIGIN = ${_addr};\n"
    "_RISCV_FLASH_SIZE = 0x10080000 - _RISCV_FLASH_ORIGIN;\n"
    "_RISCV_SRAM_ORIGIN = ${_sram_origin};\n"
    "_RISCV_SRAM_SIZE = ${_sram_size};\n"
    "_MAILBOX_SIZE = ${_mailbox_size};\n"
    "_PAL_NVM_SIZE = ${_pal_nvm_size};\n")

message(STATUS "coprocessor: RISC-V _RISCV_FLASH_ORIGIN = ${_addr} (from _riscv_boot)")
