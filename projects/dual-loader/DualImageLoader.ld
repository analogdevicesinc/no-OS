/*
 * Bootloader Linker Script for MAX32690
 *
 * This script is for a standalone bootloader at the beginning of the MAX32690's Flash.
 * It's designed to be used independently and does NOT INCLUDE the vendor's default max32690.ld.
 *
 * IMPORTANT: This script assumes stack and heap are defined and sized in the startup .S file,
 * and both are configured for UPWARD growth (i.e., __StackTop is base of stack, __HeapBase is base of heap).
 * The startup code is expected to use __StackTop (defined in .S) for SP initialization.
 */

MEMORY
{
    FLASH_BOOT (rx) : ORIGIN = 0x10000000, LENGTH = 16K /* Allocate 16KB for the bootloader. Adjust LENGTH as needed. */

    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 1M /* Full 1MB SRAM for the bootloader's general use. */

    LDR_RAM (rwx) : ORIGIN = 0x200FFFFC, LENGTH = 4 /* Dedicated small region for 'loaderState' at end of SRAM. */
}

SECTIONS
{
    /*
     * .text section: Contains program code and read-only data (like string literals).
     * The Interrupt Vector Table (ISR_VECTOR) MUST be placed at the very beginning of this section.
     */
    .text :
    {
        . = ALIGN(4);      /* Ensure 4-byte alignment for the section start */
        _text = .;         /* Define _text symbol at the start of the section */

        KEEP(*(.isr_vector)) /* Keep the Interrupt Vector Table at the beginning */
        *(.text*)          /* All code sections */
        *(.rodata*)        /* All read-only data sections ("const" variables) */
        *(.init)           /* C/C++ initialization code */
        *(.fini)           /* C/C++ finalization code */
        *(.ctors)          /* C++ global constructors */
        *(.dtors)          /* C++ global destructors */
        *(.eh_frame)       /* Exception handling frame information */
        *(.ARM.exidx)      /* ARM exception index table */

        . = ALIGN(4);      /* Ensure 4-byte alignment at the end of the section */
        _etext = .;        /* Define _etext symbol at the end of the section (where .data is loaded from) */
    } > FLASH_BOOT AT> FLASH_BOOT /* Load and execute from the bootloader's FLASH region */

    /*
     * .data section: Contains initialized global/static variables.
     * These are stored in FLASH (after .text) and copied to SRAM at bootup.
     */
    .data : AT(_etext) /* Load from FLASH at the address where .text ends */
    {
        . = ALIGN(4);     /* Ensure 4-byte alignment */
        _sdata = .;       /* Define _sdata symbol at the start of the section in SRAM */
        *(.data)          /* Standard initialized data */
        *(.data.*)        /* Other initialized data subsections */
        *(.data.overflows)/* Explicitly include this section here to resolve linker error. */
        . = ALIGN(4);
        _edata = .;       /* Define _edata symbol at the end of the section in SRAM */
    } > SRAM              /* Place the section in the main SRAM memory region */

    /*
     * .bss section: Contains uninitialized global/static variables (zero-initialized by default).
     * These are NOT stored in Flash; their memory in SRAM is zeroed out at bootup.
     */
    .bss :
    {
        . = ALIGN(4);     /* Ensure 4-byte alignment */
        _sbss = .;        /* Define _sbss symbol at the start of the section in SRAM */
        *(.bss)           /* Standard uninitialized data */
        *(.bss.*)         /* Other uninitialized data subsections */
        *(COMMON)         /* Common (uninitialized) symbols */
        . = ALIGN(4);
        _ebss = .;        /* Define _ebss symbol at the end of the section in SRAM */
    } > SRAM

    /*
     * Custom section for loader function table.
     * Placed in the bootloader's FLASH region.
     */
    .loader_function_table :
    {
        . = ALIGN(4);
        KEEP(*(.loader_function_table)) /* Keep this section even if unused */
        . = ALIGN(4);
    } > FLASH_BOOT

    /*
     * .loaderVariablesSection: Specifically for your 'loaderState' variable.
     * This is mapped to the tiny LDR_RAM region at the end of SRAM.
     */
    .loaderVariablesSection (NOLOAD) :
    {
        . = ALIGN(4);
        KEEP(*(.loaderVariables)) /* Place variables marked with __attribute__((section(".loaderVariables"))) */
        . = ALIGN(4);
    } > LDR_RAM

    /*
     * Heap section: Placed immediately after .bss, grows upwards.
     */
    .heap : ALIGN(8)
    {
        *(.heap) /* Place the .heap section defined in your .S file */
    } > SRAM

    /*
     * Stack section: Placed immediately after .heap, grows upwards.
     */
    .stack : ALIGN(8)
    {
        *(.stack) /* Place the .stack section defined in your .S file */
    } > SRAM

    /*
     * Assert to check for overall RAM fit.
     * Ensure the end of the stack does not exceed the SRAM limit.
     */
    .assert_ram_fit :
    {
        . = ASSERT(ADDR(.stack) + SIZEOF(.stack) <= (ORIGIN(SRAM) + LENGTH(SRAM)), "Error: RAM overflow! Total RAM usage exceeds SRAM limit.");
    } > SRAM

    /*
     * Provide global symbols for the startup code.
     * __StackTop and __StackLimit (from your .S file) and __HeapBase and __HeapLimit
     * are assumed to be used directly by your startup code.
     */
    PROVIDE(_sdata = ADDR(.data));
    PROVIDE(_edata = ADDR(.data) + SIZEOF(.data));
    PROVIDE(_sbss = ADDR(.bss));
    PROVIDE(_ebss = ADDR(.bss) + SIZEOF(.bss));
    PROVIDE(_etext = _etext);
}