# MAX32657 memory layout -- CAPI-LOCAL COPY.
#
# Unlike MAX32655/MAX32665/MAX32690, this file supplies NOTHING to the linker:
# every MAX32657 linker script (max32657.ld and the _s/_ns/_sla variants)
# hardcodes its FLASH/SRAM MEMORY regions, so there are no --defsym symbols to
# provide. MAX32657 is a TrustZone part instead, and what varies is the SECURITY
# ALIAS of the base addresses, not the region sizes.
#
# Bit 28 of an address selects the security alias: 0 = Non-Secure, 1 = Secure.
# The device boots Secure; this build produces a single Secure image, so the
# whole physical Flash/SRAM is used with the Secure alias bit set on the bases.
# These are pushed as C preprocessor defines (add_compile_definitions), NOT as
# linker --defsym symbols, so the firmware's __MXC_*_MEM_BASE macros agree with
# the Secure alias the .ld already uses (0x11000000 Flash, 0x30000000 SRAM)
# rather than the Non-Secure default (0x01000000 / 0x20000000).

# Physical memory settings
set(PHY_FLASH_START 0x01000000)
set(PHY_FLASH_SIZE  0x00100000) # 1 MiB
set(PHY_SRAM_START  0x20000000)
set(PHY_SRAM_SIZE   0x00040000) # 256 KiB

# Secure alias bit (bit 28).
math(EXPR SECURE_BIT "1 << 28" OUTPUT_FORMAT HEXADECIMAL)

# OR secure bit with base addresses
math(EXPR __MXC_FLASH_MEM_BASE "${PHY_FLASH_START} | ${SECURE_BIT}" OUTPUT_FORMAT HEXADECIMAL)
math(EXPR __MXC_SRAM_MEM_BASE "${PHY_SRAM_START} | ${SECURE_BIT}" OUTPUT_FORMAT HEXADECIMAL)

set(__MXC_FLASH_MEM_SIZE ${PHY_FLASH_SIZE})
set(__MXC_SRAM_MEM_SIZE ${PHY_SRAM_SIZE})

add_compile_definitions(
	__MXC_FLASH_MEM_BASE=${__MXC_FLASH_MEM_BASE}
	__MXC_FLASH_MEM_SIZE=${__MXC_FLASH_MEM_SIZE}
	__MXC_SRAM_MEM_BASE=${__MXC_SRAM_MEM_BASE}
	__MXC_SRAM_MEM_SIZE=${__MXC_SRAM_MEM_SIZE}
)
