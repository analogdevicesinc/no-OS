/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
*
* microblaze_invalidate_icache_range(unsigned int cacheaddr, unsigned int len)
*
*    Invalidate an ICache range
*
*    Parameters:
*       'cacheaddr' - address in the Icache where invalidation begins
*	'len'       - length (in bytes) worth of Icache to be invalidated
*
*
*******************************************************************************/

#include "xparameters.h"
#include "microblaze_instructions.h"

#define MICROBLAZE_MSR_ICACHE_ENABLE        0x00000020
#define MICROBLAZE_MSR_INTR_ENABLE          0x00000002

#ifndef XPAR_MICROBLAZE_ICACHE_LINE_LEN
#define XPAR_MICROBLAZE_ICACHE_LINE_LEN   1
#endif

#ifndef XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK
#define MB_VERSION_LT_v720
#endif

	.text
	.globl	microblaze_invalidate_icache_range
	.ent	microblaze_invalidate_icache_range
	.align	2

microblaze_invalidate_icache_range:
#if (XPAR_MICROBLAZE_USE_ICACHE==1) && (XPAR_MICROBLAZE_ALLOW_ICACHE_WR==1)

#ifdef MB_VERSION_LT_v720                                       /* Disable Icache and interrupts before invalidating */
	mfs	r9, rmsr
	andi	r10, r9, ~(MICROBLAZE_MSR_ICACHE_ENABLE | MICROBLAZE_MSR_INTR_ENABLE)
	mts	rmsr, r10
#endif

        BEQI    r6, L_done                                      /* Skip loop if size is zero */

        ADD	r6, r5, r6                                      /* Compute end address */
        ADDIK   r6, r6, -1

        ANDI    r6, r6, -(4 * XPAR_MICROBLAZE_ICACHE_LINE_LEN)  /* Align end down to cache line */
        ANDI    r5, r5, -(4 * XPAR_MICROBLAZE_ICACHE_LINE_LEN)  /* Align start down to cache line */

L_start:
	CMPU	r18, r5, r6                                     /* Are we at the end? */
	BLTI	r18, L_done

	wic	r5, r0                                          /* Invalidate the cache line */

#if defined (__arch64__ )
	addlik	r5, r5, (XPAR_MICROBLAZE_ICACHE_LINE_LEN * 4)   /* Increment the address by 4 */
        breai	L_start                                         /* Branch to the beginning of the loop */
#else
	brid	L_start                                         /* Branch to the beginning of the loop */
	addik	r5, r5, (XPAR_MICROBLAZE_ICACHE_LINE_LEN * 4)   /* Increment the address by 4 (delay slot) */
#endif
L_done:
	rtsd	r15, 8                                          /* Return */
#ifdef MB_VERSION_LT_v720                                       /* restore MSR only for MB version < v7.20 */
	mts	rmsr, r9
#else
        nop
#endif

#else
	rtsd	r15, 8                                          /* Return */
        nop
#endif
.end	microblaze_invalidate_icache_range
