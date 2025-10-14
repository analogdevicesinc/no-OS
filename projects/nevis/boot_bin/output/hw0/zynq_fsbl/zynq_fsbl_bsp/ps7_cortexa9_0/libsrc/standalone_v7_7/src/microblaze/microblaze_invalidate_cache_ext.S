/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* microblaze_invalidate_cache_ext()
*
*Invalidate the entire L2 Cache
*
*
*******************************************************************************/

#include "xparameters.h"
#include "microblaze_instructions.h"

#define XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN	16
#define CACHEABLE_REGION_SIZE (XPAR_MICROBLAZE_DCACHE_HIGHADDR - XPAR_MICROBLAZE_DCACHE_BASEADDR)

	.text
	.globl	microblaze_invalidate_cache_ext
	.ent	microblaze_invalidate_cache_ext
	.align	2

microblaze_invalidate_cache_ext:

#if ((XPAR_MICROBLAZE_INTERCONNECT==3) && (XPAR_MICROBLAZE_USE_DCACHE==1))
	ADDIK	r5, r0, XPAR_MICROBLAZE_DCACHE_BASEADDR & (-(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN))

	ADDIK	r6, r0, CACHEABLE_REGION_SIZE-(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)
	ANDI	r6, r6, -(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)

Loop_start:
	wdc.ext.clear	r5, r6
#if defined (__arch64__ )
	addlik	r6, r6,-(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)
	beagei r6, Loop_start
#else
	bgtid	r6,Loop_start
	addik	r6, r6,-(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)
#endif
#endif
	rtsd	r15, 8
	nop

	.end	microblaze_invalidate_cache_ext
