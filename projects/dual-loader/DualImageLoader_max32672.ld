/******************************************************************************
 * Bootloader-specific sections to be used with default linker script
 * 
 * Usage: LSCRIPT = max32672.ld bootloader_sections.ld
 * 
 * This script only ADDS new sections, doesn't override existing ones
 ******************************************************************************/

/* Define additional memory regions that don't conflict with default */
MEMORY {
    /* Function table at end of first 8KB page */
    FLASH_BOOT_TABLE (rx) : ORIGIN = 0x10001F80, LENGTH = 128
    /* Loader state at very end of SRAM */
    LDR_STATE (rwx)       : ORIGIN = 0x20027FFC, LENGTH = 4
}

/* Add our custom sections without overriding default ones */
SECTIONS {
    /* Place loader function table in its own region */
    .loader_function_table :
    {
        . = ALIGN(4);
        KEEP(*(.loader_function_table))
        . = ALIGN(4);
    } > FLASH_BOOT_TABLE
    
    /* Loader state variable in dedicated RAM region */
    .loaderVariablesSection (NOLOAD) :
    {
        . = ALIGN(4);
        KEEP(*(.loaderVariables))
        . = ALIGN(4);
    } > LDR_STATE
}

/* Ensure bootloader doesn't exceed 8KB - 128 bytes */
ASSERT((_etext <= 0x10001F80), "Bootloader text exceeds 8KB - 128 bytes!")

/* Ensure bootloader's data+bss doesn't exceed the reserved 4KB */
/* _ebss is the end of BSS section, which should be the last RAM usage by bootloader */
ASSERT((_ebss <= 0x20001000), "Bootloader RAM usage exceeds reserved 4KB! End of BSS: _ebss. Reduce bootloader RAM usage or increase reserved RAM in app linker scripts.")