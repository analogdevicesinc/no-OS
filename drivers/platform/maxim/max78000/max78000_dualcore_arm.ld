/******************************************************************************
 * MAX78000 dual-core ARM (Cortex-M4F, CPU0) linker script.
 *
 * Use this instead of the default max78000.ld when the ARM image embeds a RISC-V
 * (CPU1) coprocessor firmware that runs in place from flash. It does two things
 * the single-core script does not:
 *
 *   1. Partitions RAM so the ARM cannot corrupt the RISC-V. ARM data/bss go in
 *      SRAM0 and the ARM stack/heap in SRAM1 (__StackTop = end of SRAM1), leaving
 *      SRAM2 entirely to the RISC-V (its data/bss/stack). SRAM3 is unused.
 *
 *   2. Reserves a page-aligned flash region for the embedded RISC-V image and
 *      exports its start as _riscv_boot. `EXCLUDE_FILE (*riscv.o) *(.text*)` keeps
 *      the RISC-V bytes out of ARM code; `.riscv_flash` KEEPs them at _riscv_boot.
 *      The ARM boots the coprocessor by writing &_riscv_boot to FCR->urvbootaddr.
 *
 * _riscv_boot is placed after all ARM content, so its address does not depend on
 * whether the RISC-V blob is present. That lets the build link the ARM once to
 * discover _riscv_boot, build the RISC-V to run there, then relink with the blob.
 ******************************************************************************/

__FlashStart = 0x10000000;
__FlashLength = 0x00080000;
INCLUDE "common.ld"

SECTIONS {
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        EXCLUDE_FILE (*riscv.o) *(.text*)    /* ARM code only; RISC-V goes to .riscv_flash */

        KEEP(*(.init))
        KEEP(*(.fini))

        KEEP(*(.rodata*))  /* read-only data: "const" */

        /* C++ Exception handling */
        KEEP(*(.eh_frame*))
        _etext = .;
    } > FLASH

    /* Binary import!!! */
    .bin_storage :
    {
       FILL(0xFF)
      _bin_start_ = .;
      KEEP(*(.bin_storage_img))
      _bin_end_ = .;
      . = ALIGN(4);
    } > FLASH

    /* it's used for C++ exception handling      */
    /* we need to keep this to avoid overlapping */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    .shared :
    {
        . = ALIGN(4);
        _shared = .;
        *(.mailbox*)
        . = ALIGN(4);
        *(.shared*)     /*read-write zero initialized data: uninitialzed global variable*/
        _eshared = ALIGN(., 4);
    } > SRAM2 AT>FLASH
    __shared_data = LOADADDR(.shared);

    .data :
    {
        _data = ALIGN(., 4);
        *(.data*)           /*read-write initialized data: initialized global variable*/
        *(.spix_config*)    /* SPIX configuration functions need to be run from SRAM */
        *(.flashprog*)      /* Flash program */

        /* These array sections are used by __libc_init_array to call static C++ constructors */
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        _edata = ALIGN(., 4);
    } > SRAM0 AT>FLASH
    __load_data = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)     /*read-write zero initialized data: uninitialzed global variable*/
        *(COMMON)
        _ebss = ALIGN(., 4);
    } > SRAM0


    /* ARM stack/heap live at the top of SRAM1, leaving SRAM2 for the RISC-V. */
    __StackTop = ORIGIN(SRAM1) + LENGTH(SRAM1);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > SRAM1

    .heap (COPY):
    {
        . = ALIGN(4);
        *(.heap*)
        __HeapLimit = ABSOLUTE(__StackLimit);
    } > SRAM1

    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= _ebss, "region RAM overflowed with stack")

    /* Embedded RISC-V image. riscv.o (the .incbin-wrapped firmware binary) is
     * KEPT here at a flash-page-aligned address; _riscv_boot is that address, the
     * value the ARM writes to FCR->urvbootaddr. The +0x100 offset keeps the image
     * off the very first page boundary, matching the flash page geometry. */
    .riscv_flash :
    {
        . = ALIGN(256);
        . += 0x100;
        _riscv_boot = .;
        KEEP(*riscv.o (.text*))
    } > FLASH
}
