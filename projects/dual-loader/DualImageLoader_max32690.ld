/******************************************************************************
 * MAX32690 Bootloader-specific sections to be used with default linker script
 * 
 * Usage: LSCRIPT = max32690.ld DualImageLoader_max32690.ld
 * 
 * This script only ADDS new sections, doesn't override existing ones
 ******************************************************************************/

/* Define additional memory regions that don't conflict with default */
MEMORY {
    /* Function table at end of first 16KB page */
    FLASH_BOOT_TABLE (rx) : ORIGIN = 0x10003F80, LENGTH = 128
    /* Loader state at very end of SRAM (256KB) */
    LDR_STATE (rwx)       : ORIGIN = 0x2003FFFC, LENGTH = 4
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

/* Ensure bootloader doesn't exceed 16KB - 128 bytes */
ASSERT((_etext <= 0x10003F80), "Bootloader text exceeds 16KB - 128 bytes!")