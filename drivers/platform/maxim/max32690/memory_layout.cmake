# Define base memory regions for ARM and RISC-V
set(_ARM_FLASH_ORIGIN       0x10000000)
set(_ARM_FLASH_SIZE         0x340000)
set(_ARM_SRAM_ORIGIN        0x20000000)
set(_ARM_SRAM_SIZE          0x100000)

set(_RISCV_FLASH_ORIGIN 0x10300000)
set(_RISCV_FLASH_SIZE   0x40000)
set(_RISCV_SRAM_ORIGIN  0x20100000)
set(_RISCV_SRAM_SIZE    0x20000)

set(_PAL_NVM_SIZE       0x0)

# Select memory regions based on CPU core
if(DEFINED CPU_CORE AND CPU_CORE STREQUAL "riscv")
	# RISC-V core uses dedicated regions
	set(_FLASH_ORIGIN       ${_RISCV_FLASH_ORIGIN})
	set(_FLASH_SIZE         ${_RISCV_FLASH_SIZE})
	set(_SRAM_ORIGIN        ${_RISCV_SRAM_ORIGIN})
	set(_SRAM_SIZE          ${_RISCV_SRAM_SIZE})
	set(_MAILBOX_SIZE       64)  # Enable mailbox for IPC
else()
	# ARM core uses main regions (default)
	set(_FLASH_ORIGIN       ${_ARM_FLASH_ORIGIN})
	set(_FLASH_SIZE         ${_ARM_FLASH_SIZE})
	set(_SRAM_ORIGIN        ${_ARM_SRAM_ORIGIN})
	set(_SRAM_SIZE          ${_ARM_SRAM_SIZE})
	set(_MAILBOX_SIZE       0)   # No mailbox needed for ARM-only
endif()

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
