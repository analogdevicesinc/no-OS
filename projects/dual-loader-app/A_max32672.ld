MEMORY {
    /* Bank 0: Bootloader (8K) + Image A (128K) */
    /* Image A starts after 1 page (8K) bootloader */
    FLASH (rx) : ORIGIN = 0x10002000, LENGTH = 504K - 16      /* Bank 0: 512KB - 8KB bootloader = 504KB for image */
    FOOTER (rx) : ORIGIN = 0x1007FFF0, LENGTH = 16  /* End of bank 0 */
    /* Start SRAM after bootloader's RAM usage (0x1000 bytes) */
    SRAM (rwx) : ORIGIN = 0x20001000, LENGTH = 160K - 0x1000 - 4 /* MAX32672 has 160KB SRAM, reserve first 4KB for bootloader + 4 bytes for loader state */
}

/* Bootloader API addresses - adjusted for 8K page size */
dl_start_download           = 0x10001f80;
dl_download_image_block     = 0x10001f84;
dl_abort_upgrade            = 0x10001f88;
dl_complete_download        = 0x10001f8c;
dl_run_image_a              = 0x10001f90;
dl_run_image_b              = 0x10001f94;
dl_commit_image_a           = 0x10001f98;
dl_commit_image_b           = 0x10001f9c;
dl_get_image_status         = 0x10001fa0;
dl_get_version              = 0x10001fa4;
dl_get_build_date           = 0x10001fa8;

SECTIONS {
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)    /* program code */
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

    .data :
    {
        _data = ALIGN(., 4);
        *(.data*)           /*read-write initialized data: initialized global variable*/
        *(.spix_config*)    /* SPIX configuration functions need to be run from SRAM */

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

    .app_footer :
    {
        QUAD(0);
    } > FOOTER

    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)     /*read-write zero initialized data: uninitialzed global variable*/
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
        *(.heap*)
        __HeapLimit = ABSOLUTE(__StackLimit);
    } > SRAM

    PROVIDE(__stack = __StackTop);

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= _ebss, "region RAM overflowed with stack")
}
