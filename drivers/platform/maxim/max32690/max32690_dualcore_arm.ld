/******************************************************************************
 * MAX32690 dual-core ARM (Cortex-M4F, CPU0) linker script.
 *
 * Use this instead of the default max32690.ld when the ARM image embeds a
 * RISC-V (CPU1) coprocessor firmware that runs in place from flash. It does
 * two things the single-core script does not:
 *
 *   1. Partitions RAM and Flash according to memory_layout.cmake:
 *      - ARM:    Flash 0x10000000-0x10340000 (3.25MB)
 *                SRAM  0x20000000-0x20100000 (1MB)
 *      - RISC-V: Flash 0x10300000-0x10340000 (256KB, embedded in ARM region)
 *                SRAM  0x20100000-0x20120000 (128KB, separate from ARM)
 *
 *   2. Reserves a flash region for the embedded RISC-V image and exports its
 *      start as _riscv_boot. `EXCLUDE_FILE (*riscv.o) *(.text*)` keeps the
 *      RISC-V bytes out of ARM code; `.riscv_flash` KEEPs them at _riscv_boot.
 *      The ARM boots the coprocessor by writing &_riscv_boot to FCR->urvbootaddr.
 *
 * _riscv_boot is placed after all ARM content, so its address does not depend
 * on whether the RISC-V blob is present. That lets the build link the ARM once
 * to discover _riscv_boot, build the RISC-V to run there, then relink with blob.
 ******************************************************************************/

__FlashStart = 0x10000000;
__FlashLength = 0x00340000;  /* 3.25MB total for ARM */
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

    /* Shared memory for IPC (mailbox) between ARM and RISC-V */
    .shared :
    {
        . = ALIGN(4);
        _shared = .;
        *(.mailbox*)
        . = ALIGN(4);
        *(.shared*)     /*read-write zero initialized data: uninitialzed global variable*/
        _eshared = ALIGN(., 4);
    } > SRAM AT>FLASH
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
    } > SRAM AT>FLASH
    __load_data = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)     /*read-write zero initialized data: uninitialzed global variable*/
        *(COMMON)
        _ebss = ALIGN(., 4);
    } > SRAM


    /* ARM stack/heap stay in ARM SRAM (0x20000000-0x20100000).
     * RISC-V uses separate SRAM region (0x20100000-0x20120000). */
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
        *(.heap*)
        __HeapLimit = ABSOLUTE(__StackLimit);
    } > SRAM

    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= _ebss, "region RAM overflowed with stack")

    /* Embedded RISC-V image. riscv.o (the .incbin-wrapped firmware binary) is
     * KEPT here at the designated flash address (0x10300000). _riscv_boot is
     * that address, the value the ARM writes to FCR->urvbootaddr. The alignment
     * and offset match flash page geometry for MAX32690. */
    .riscv_flash 0x10300000 :
    {
        . = ALIGN(8192);  /* Flash page alignment for MAX32690 */
        _riscv_boot = .;
        KEEP(*riscv.o (.text*))
    } > FLASH
}
