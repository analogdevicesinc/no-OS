/***************************************************************************//**
 *   @file   riscvrx_cache.c
 *   @brief  Linker of caching for Lattice RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "riscvrx_cache.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief data cache invalidate range Lattice specific function.
 * @param addr - base address
 * @param range - size of the memory region
 */
void lattice_dcache_invalidate_range(uint32_t addr, uint32_t range)
{
	range = range + (addr & 0x1F);
	for (int i = 0; i < range >> 2; i += 8) {
		cache_invalidate(((uint32_t *) addr) + i);
	}
}
