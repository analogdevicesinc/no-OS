/*
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * MAX32690 CPU0 linker script for builds that embed a CPU1 (RISC-V) image.
 *
 * The SDK script already reserves the dedicated RISC-V flash bank as
 * RISCV_FLASH and places the embedded image (riscv.o) in .riscv_flash. CPU1
 * boots from the start of that bank, exported as _riscv_boot for the
 * coprocessor build and for capi_coprocessor_set_vector_base().
 */

INCLUDE max32690.ld

/* RISCV_FLASH starts at _RISCV_FLASH_ORIGIN (passed with --defsym). */
_riscv_boot = _RISCV_FLASH_ORIGIN;
