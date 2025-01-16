/*******************************************************************************
 * Copyright (C) 2020 Maxim Integrated Products, Inc., All Rights Reserved.
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
HEADER_SIZE = 0x200;
HEADER_ORIGIN = 0x10000000;

FLASH_SIZE = 0x00300000;
FILESYSTEM_SIZE = 0x28000;
ROM_SIZE = 0x00010000;
SRAM_SIZE = 0x00100000;

SRAM_ORIGIN = 0x20000000;
ROM_ORIGIN = 0x00000000;
FIRMWARE_ORIGIN = HEADER_ORIGIN + HEADER_SIZE;
FIRMWARE_SIZE = FLASH_SIZE - FILESYSTEM_SIZE - HEADER_SIZE;
FILESYSTEM_ORIGIN = FIRMWARE_ORIGIN + FIRMWARE_SIZE;

MEMORY {
    HEADER (rx): ORIGIN = HEADER_ORIGIN, LENGTH = HEADER_SIZE /* 256B SLA Header, start on next page */
    FIRMWARE(rx) : ORIGIN = FIRMWARE_ORIGIN, LENGTH = FIRMWARE_SIZE /* ~3MB for application */
    FILESYSTEM(rw) : ORIGIN = FILESYSTEM_ORIGIN, LENGTH = FILESYSTEM_SIZE /* 160KB for filesystem */
    SRAM (rwx) : ORIGIN = SRAM_ORIGIN, LENGTH = SRAM_SIZE
}

_fs_start = ORIGIN(FILESYSTEM);						/* Start of the filesystem */
_fs_len = LENGTH(FILESYSTEM);						/* Size of the filesystem */
_fs_end = ORIGIN(FILESYSTEM) + LENGTH(FILESYSTEM);	/* End of the filesystem */
FS_ORIGIN_PAGE_OFFSET = _fs_start % 0x4000;
ASSERT(!FS_ORIGIN_PAGE_OFFSET, "Filesystem page should start from beginning of a page");

/* Added Oct 9, 2018 to go to correct reset vector. */
ENTRY(Reset_Handler)
PROVIDE( _start_SWAP = (((Reset_Handler) >> 24) | (((Reset_Handler) & 0x00FF0000) >> 8) | (((Reset_Handler) & 0x0000FF00) << 8) | ((Reset_Handler) << 24)));
PROVIDE_HIDDEN( _SLA_Size = _endimage - __end_header );
PROVIDE( _SLA_Size_SWAP = (((_SLA_Size) >> 24) | (((_SLA_Size) & 0x00FF0000) >> 8) | (((_SLA_Size) & 0x0000FF00) << 8) | ((_SLA_Size) << 24)));

/* Sections Definitions */
SECTIONS {
	 .sb_sla_header : ALIGN(4)
    {
		FILL(0xFF)
		KEEP(*(.sb_sla_header)) /* Header for ROM code */
		 __end_header = . ;
		. = ALIGN(512);
    } >HEADER

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
    } > FIRMWARE

    /* it's used for C++ exception handling      */
    /* we need to keep this to avoid overlapping */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
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
    _enddata = LOADADDR(.data)+SIZEOF(.data);

    .sb_sla_trailer : AT(_enddata)
    {
        KEEP(*(.sb_sla_trailer))
        /* Align image with 16 byte boundary to conform to flash encryption block size. */
        FILL(0xDEADC0DE);
        /* NOTE: The FILL and ALIGN will not work unless something is written to the section.  So, we use LONG. */
        LONG(0xDEADC0DE);
        . = ALIGN(16);
    }  > FIRMWARE
    _endimage = LOADADDR(.sb_sla_trailer)+SIZEOF(.sb_sla_trailer);
    .sig :
    {
        KEEP(*(.sig))
        LONG(0xDEADBEEF);

    }  > FIRMWARE
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
