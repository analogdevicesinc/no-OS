/******************************************************************************
 * Copyright (C) 2023 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

FLASH_SIZE = 0x00060000; /* 384kB */
FILESYSTEM_SIZE = 0x28000; /* 160kB */
SRAM_ORIGIN = 0x20000000;
SRAM_SIZE   = 0x20000; /* 128kB to accomodate ECC */

FIRMWARE_ORIGIN = 0x10000000;
FIRMWARE_SIZE = FLASH_SIZE - FILESYSTEM_SIZE;
FILESYSTEM_ORIGIN = FIRMWARE_ORIGIN + FIRMWARE_SIZE;
FILESYSTEM_PAGE_OFFSET = FILESYSTEM_ORIGIN % 0x4000;
ASSERT(!FILESYSTEM_PAGE_OFFSET, "Filesystem page should start from beginning of a page");

MEMORY {
    FIRMWARE(rx) : ORIGIN = FIRMWARE_ORIGIN, LENGTH = FIRMWARE_SIZE
    FILESYSTEM(rw) : ORIGIN = FILESYSTEM_ORIGIN, LENGTH = FILESYSTEM_SIZE
    SRAM (rwx) : ORIGIN = SRAM_ORIGIN, LENGTH = SRAM_SIZE
}

_fs_start = ORIGIN(FILESYSTEM);						/* Start of the filesystem */
_fs_len = LENGTH(FILESYSTEM);						/* Size of the filesystem */
_fs_end = ORIGIN(FILESYSTEM) + LENGTH(FILESYSTEM);	/* End of the filesystem */

SECTIONS {

    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)    /* program code */
        *(.rodata*)  /* read-only data: "const" */

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        /* C++ Exception handling */
        KEEP(*(.eh_frame*))
        _etext = .;
    } > FIRMWARE

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FIRMWARE

    /* it's used for C++ exception handling      */
    /* we need to keep this to avoid overlapping */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FIRMWARE

    /* use it with uninitialzed data only */
    /* use Sys_CheckRamRetentionAddr() for initialization decision */
    .ramRetentionBlock 0x20000000 (COPY):
    {
        _start_retention_data = ALIGN(., 4);
        KEEP(*(.ram_retention_prio1))     /* 1st priority retained data (must-preserve) */
        KEEP(*(.ram_retention_prio2))     /* 2nd priority retained data */
        _end_retention_data = ALIGN(., 4);
    } > SRAM

    .uclStackBlock 0x20008000 (COPY):
    {
        KEEP(*(.uclStackSection))
    } > SRAM

    .data :
    {
        _data = ALIGN(., 4);
        *(vtable)
        *(.data*)           /*read-write initialized data: initialized global variable*/
        *(.spix_config*)    /* SPIX configuration functions need to be run from SRAM */
        *(.flashprog*)      /* Flash program */
        *(.ram_code*)
        *(.got.plt) *(.got)

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
    } > SRAM AT>FIRMWARE
    __load_data = LOADADDR(.data);

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

    /* Check if ram retention section exceeds SRAM0 block limit */
    ASSERT(_end_retention_data <= 0x20008000, "ram retention section overflowed")
}