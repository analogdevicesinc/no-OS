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

file(WRITE "${OUT_FILE}"
    "/* Auto-generated -- do not edit. RISC-V flash origin from the ARM map. */\n"
    "__FlashStart = ${_addr};\n"
    "__FlashLength = 0x10080000 - __FlashStart;\n")

message(STATUS "coprocessor: RISC-V __FlashStart = ${_addr} (from _riscv_boot)")
