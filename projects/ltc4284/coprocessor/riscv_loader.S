/*
 * Auto-generated -- do not edit. Wraps the built RISC-V firmware binary as raw
 * data in an object the ARM linker can place. The dual-core ARM linker script
 * KEEPs this object (matched by the name riscv.o) in its .riscv_flash section at
 * _riscv_boot and excludes it from the ARM .text. The bytes are RISC-V code; the
 * RISC-V core executes them in place from flash at _riscv_boot.
 */
    .section .text
    .globl _binary_riscv_bin_start
    .globl _binary_riscv_bin_end
_binary_riscv_bin_start:
    .incbin "/home/cj/no-OS/projects/ltc4284/coprocessor/riscv.bin"
_binary_riscv_bin_end:
