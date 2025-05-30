/******************************************************************************
 * MAX32672 Bootloader Linker Script
 * 
 * Memory Layout:
 * - Bank 0 (0x10000000): Bootloader (1 page = 8KB)
 * - Bank 0 (0x10002000): Image A
 * - Bank 1 (0x10080000): Commit area (1 page = 8KB)
 * - Bank 1 (0x10082000): Image B
 ******************************************************************************/

MEMORY {
    FLASH_BOOT (rx)     : ORIGIN = 0x10000000, LENGTH = 8K - 128  /* Bootloader minus function table */
    FLASH_BOOT_END (rx) : ORIGIN = 0x10001F80, LENGTH = 128       /* Function table at end of page */
    SRAM (rwx)          : ORIGIN = 0x20000000, LENGTH = 160K - 4  /* 160KB SRAM with ECC minus loader state */
    LDR_RAM (rwx)       : ORIGIN = 0x20027FFC, LENGTH = 4         /* Loader state at end of SRAM */
}

SECTIONS {
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)    /* program code */
        *(.rodata*)  /* read-only data: "const" */
        *(.init)
        *(.fini)
        *(.ctors)
        *(.dtors)
        *(.eh_frame)
        *(.ARM.exidx)
        
        . = ALIGN(4);
        _etext = .;
    } > FLASH_BOOT AT> FLASH_BOOT

    /* Loader function table in dedicated region */
    .loader_function_table :
    {
        . = ALIGN(4);
        KEEP(*(.loader_function_table))
        . = ALIGN(4);
    } > FLASH_BOOT_END

    /* Store load address for data section */
    __load_data = LOADADDR(.data);

    .data : AT(_etext)
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data.*)
        *(.data.overflows)
        
        /* Flash programming functions must run from RAM */
        *(.flashprog*)
        
        . = ALIGN(4);
        _edata = .;
    } > SRAM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* Loader state variable in dedicated RAM region */
    .loaderVariablesSection (NOLOAD) :
    {
        . = ALIGN(4);
        KEEP(*(.loaderVariables))
        . = ALIGN(4);
    } > LDR_RAM

    /* Heap section */
    .heap : ALIGN(8)
    {
        *(.heap)
    } > SRAM

    /* Stack section */
    .stack : ALIGN(8)
    {
        *(.stack)
    } > SRAM

    /* Stack configuration */
    __StackTop = ORIGIN(SRAM) + LENGTH(SRAM);
    __StackLimit = __StackTop - 0x1000;  /* 4KB stack */
    
    PROVIDE(__stack = __StackTop);

    /* Provide symbols for startup code */
    PROVIDE(_sdata = ADDR(.data));
    PROVIDE(_edata = ADDR(.data) + SIZEOF(.data));
    PROVIDE(_sbss = ADDR(.bss));
    PROVIDE(_ebss = ADDR(.bss) + SIZEOF(.bss));
    PROVIDE(_etext = _etext);

    /* Ensure we don't exceed loader space */
    ASSERT((_etext <= (ORIGIN(FLASH_BOOT) + LENGTH(FLASH_BOOT))), "Bootloader exceeds allocated flash space")
    ASSERT(ADDR(.stack) + SIZEOF(.stack) <= (ORIGIN(SRAM) + LENGTH(SRAM)), "RAM overflow! Total RAM usage exceeds SRAM limit.")
}