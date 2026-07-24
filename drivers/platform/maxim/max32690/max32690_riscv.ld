/******************************************************************************
 * MAX32690 RISC-V Linker Script for no-OS Dual-Core Builds
 *
 * This wrapper script includes the auto-generated flash/SRAM symbols from
 * the two-pass link (buildrv/common_riscv.ld) and then includes the stock
 * Maxim SDK RISC-V linker script.
 *
 * The ARM premap determines where the RISC-V image starts in flash, and
 * gen_common_riscv_ld.cmake writes those definitions to buildrv/common_riscv.ld
 * at build time.
 ******************************************************************************/

/* Include auto-generated symbols from two-pass build */
INCLUDE "buildrv/common_riscv.ld"

/* Calculate flash and SRAM regions for SDK linker script */
FLASH_MEM_SIZE = _RISCV_FLASH_SIZE - _PAL_NVM_SIZE;
FLASH_MEM_ORIGIN = _RISCV_FLASH_ORIGIN - _PAL_NVM_SIZE;

SRAM_MEM_ORIGIN = _RISCV_SRAM_ORIGIN + _MAILBOX_SIZE*2;
SRAM_MEM_SIZE =  _RISCV_SRAM_SIZE - _MAILBOX_SIZE*2;

MAILBOX_ORIGIN = _RISCV_SRAM_ORIGIN;

MEMORY {
    FLASH      (rx) : ORIGIN = FLASH_MEM_ORIGIN, LENGTH = FLASH_MEM_SIZE
    PAL_NVM_DB (r)  : ORIGIN = FLASH_MEM_ORIGIN + FLASH_MEM_SIZE, LENGTH = _PAL_NVM_SIZE
    MAILBOX_0  (rw) : ORIGIN = MAILBOX_ORIGIN, LENGTH = _MAILBOX_SIZE
    MAILBOX_1  (rw) : ORIGIN = MAILBOX_ORIGIN + _MAILBOX_SIZE, LENGTH = _MAILBOX_SIZE
    SRAM      (rwx) : ORIGIN = SRAM_MEM_ORIGIN, LENGTH = SRAM_MEM_SIZE
}

SECTIONS {

    .text :
    {
        _text = ALIGN(.,13);
        KEEP(*(.isr_vector))
        KEEP(*(.c_iv))
        *(.text*)    /* program code */

        *(.srodata*) /* read-only data: "const" */
        *(.rodata*)  /* read-only data: "const" */

        KEEP(*(.init))
        KEEP(*(.fini))

        /* C++ Exception handling */
        KEEP(*(.eh_frame*))
        _etext = .;
    } > FLASH

    /* it's used for C++ exception handling      */
    /* we need to keep this to avoid overlapping */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    /* Global pointer for RISC-V relaxation optimizations */
    .sdata :
    {
        __global_pointer$ = . + 0x800;
        _sdata = .;
        *(.sdata*)
        _esdata = .;
    } > SRAM AT>FLASH
    __load_sdata = LOADADDR(.sdata);

    .data :
    {
        _data = ALIGN(., 4);
        *(.data*)           /*read-write initialized data: initialized global variable*/
        _edata = ALIGN(., 4);
    } > SRAM AT>FLASH
    __load_data = LOADADDR(.data);

    .sbss (NOLOAD):
    {
        _sbss = .;
        *(.sbss*)
        _esbss = .;
    } > SRAM

    .bss (NOLOAD):
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)     /*read-write zero initialized data: uninitialized global variable*/
        *(COMMON)
        _ebss = ALIGN(., 4);
    } > SRAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(SRAM) + LENGTH(SRAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > SRAM

    .heap (COPY):
    {
        . = ALIGN(4);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        *(.heap*)
        __HeapLimit = ABSOLUTE(__StackLimit);
    } > SRAM

    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= _ebss, "region RAM overflowed with stack")
}
