/******************************************************************************
 * MAX32690 Common Memory Map for Dual-Core ARM + RISC-V Configuration
 *
 * This file defines MEMORY regions for dual-core builds using linker symbols
 * provided by memory_layout.cmake via -Wl,--defsym flags.
 *
 * Memory Layout:
 *   - ARM Flash:   0x10000000-0x10340000 (3.25MB)
 *   - ARM SRAM:    0x20000000-0x20100000 (1MB)
 *   - RISC-V Flash: 0x10300000-0x10340000 (256KB, embedded in ARM region)
 *   - RISC-V SRAM:  0x20100000-0x20120000 (128KB, separate)
 *
 * Note: Unlike MAX78000 which uses SRAM0/SRAM1/SRAM2 naming, MAX32690 has
 * a single contiguous SRAM region partitioned between ARM and RISC-V.
 ******************************************************************************/

_FlashLength_ = __FlashLength;

MEMORY {
    ROM     (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00020000 /* 128kB - ROM */

    /* ARM Flash region (0x10000000-0x10340000, 3.25MB) */
    FLASH   (rx)  : ORIGIN = __FlashStart, LENGTH = _FlashLength_

    /* ARM SRAM region (0x20000000-0x20100000, 1MB)
     * Used for ARM data, bss, stack, and heap.
     * RISC-V uses separate SRAM at 0x20100000-0x20120000. */
    SRAM    (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00100000  /* 1MB - ARM SRAM */
}
