/*******************************************************************************
 *   @file   riscvrx_cache.h
 *   @brief  Header containing types used in caching for RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef LATTICE_CACHE_H_
#define LATTICE_CACHE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "cache.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief data cache invalidate range Lattice specific function.
 * @param addr - base address
 * @param range - size of the memory region
 */
void lattice_dcache_invalidate_range(uint32_t addr, uint32_t range);

#endif
