# Bit 28 of an address selects the security alias: 0 = Non-Secure, 1 = Secure.
# The device boots Secure; this build produces a single Secure image, so the
# whole physical Flash/SRAM is used with the Secure alias bit set on the bases.

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
