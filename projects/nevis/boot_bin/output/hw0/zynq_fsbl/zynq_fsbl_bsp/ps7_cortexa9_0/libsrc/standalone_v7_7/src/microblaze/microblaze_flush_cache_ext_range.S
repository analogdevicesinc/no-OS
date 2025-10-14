/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* microblaze_flush_cache_ext_range (unsigned int cacheaddr, unsigned int len)
*
*Flush a L2 Cache range
*
*Parameters:
*	'cacheaddr' - address in the L2 cache where the flush begins
*	'len    '   - length (in bytes) worth of L2 cache to be flushed
*
*******************************************************************************/

#include "xparameters.h"
#include "microblaze_instructions.h"

#define XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN	16

	.text
	.globl	microblaze_flush_cache_ext_range
	.ent	microblaze_flush_cache_ext_range
	.align	2

microblaze_flush_cache_ext_range:
#if ((XPAR_MICROBLAZE_INTERCONNECT==3) && (XPAR_MICROBLAZE_USE_DCACHE==1))
	beqi	r6, Loop_done

	ADDIK	r6, r6, -1
	ADD	r6, r5, r6

	ANDI	r6, r6, -(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)
	ANDI	r5, r5, -(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)

	RSUBK	r6, r5, r6
Loop_start:
	wdc.ext.flush r5, r6
#if defined (__arch64__ )
	addlik	r6, r6, -(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)
	beagei r6, Loop_start
#else
	bneid	r6, Loop_start
	addik	r6, r6, -(4 * XPAR_MICROBLAZE_EXT_CACHE_LINE_LEN)
#endif

Loop_done:
#endif
	rtsd	r15, 8
	nop

	.end	microblaze_flush_cache_ext_range
