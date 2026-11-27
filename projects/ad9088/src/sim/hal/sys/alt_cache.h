/***************************************************************************//**
 *   @file   alt_cache.h
 *   @brief  Simulation stand-in for the Nios V HAL cache API.
 *
 *           The host has coherent caches, so the flushes are no-ops.
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef SIM_ALT_CACHE_H_
#define SIM_ALT_CACHE_H_

static inline void alt_dcache_flush(void *start, unsigned int len)
{
	(void)start;
	(void)len;
}

static inline void alt_dcache_flush_no_writeback(void *start,
		unsigned int len)
{
	(void)start;
	(void)len;
}

static inline void alt_dcache_flush_all(void)
{
}

#endif /* SIM_ALT_CACHE_H_ */
