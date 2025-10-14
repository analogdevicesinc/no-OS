/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* microblaze_flush_dcache_range (unsigned int cacheaddr, unsigned int len)
*
*    Flush a L1 DCache range
*
*    Parameters:
*       'cacheaddr' - address in the Dcache where the flush begins
*	'len    '   - length (in bytes) worth of Dcache to be flushed
*
*******************************************************************************/

#include "xparameters.h"
#include "microblaze_instructions.h"

#define MICROBLAZE_MSR_DCACHE_ENABLE        0x00000080
#define MICROBLAZE_MSR_INTR_ENABLE          0x00000002

#ifndef XPAR_MICROBLAZE_DCACHE_LINE_LEN
#define XPAR_MICROBLAZE_DCACHE_LINE_LEN   1
#endif

#ifndef XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK
#define MB_VERSION_LT_v720
#define MB_HAS_WRITEBACK_SET 0
#else
#define MB_HAS_WRITEBACK_SET XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK
#endif

	.text
	.globl	microblaze_flush_dcache_range
	.ent	microblaze_flush_dcache_range
	.align	2

microblaze_flush_dcache_range:
#if (XPAR_MICROBLAZE_USE_DCACHE==1) && (XPAR_MICROBLAZE_ALLOW_DCACHE_WR==1)

#ifdef MB_VERSION_LT_v720                                       /* Disable Dcache and interrupts before invalidating */
	mfs	r9, rmsr
	andi	r10, r9, ~(MICROBLAZE_MSR_DCACHE_ENABLE | MICROBLAZE_MSR_INTR_ENABLE)
	mts	rmsr, r10
#endif

        beqi    r6, L_done                                      /* Skip loop if size is zero */

        ADD	r6, r5, r6                                      /* Compute end address */
        ADDIK   r6, r6, -1

        ANDI    r6, r6, -(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN)  /* Align end down to cache line */
        ANDI    r5, r5, -(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN)  /* Align start down to cache line */

#if MB_HAS_WRITEBACK_SET == 0                                   /* Use a different scheme for MB version < v7.20 or when caches are write-through */

L_start:
	CMPU	r18, r5, r6                                     /* Are we at the end? */
	BLTI	r18, L_done

	wdc     r5, r0                                          /* Invalidate the cache line */

#if defined (__arch64__ )
	addlik	r5, r5, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 */
	breai	L_start                                         /* Branch to the beginning of the loop */
#else
	brid	L_start                                         /* Branch to the beginning of the loop */
	addik	r5, r5, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 (delay slot) */
#endif
#else
        RSUBK   r6, r5, r6
                                                                /* r6 will now contain (count of bytes - (4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN)) */
L_start:
        wdc.flush r5, r6                                        /* Flush the cache line */
#if defined (__arch64__ )
        addlik   r6, r6, -(XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)
        beagei r6, L_start
#else
        bneid   r6, L_start
        addik   r6, r6, -(XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)
#endif
#endif

L_done:
	rtsd	r15, 8
#ifdef MB_VERSION_LT_v720                                       /* restore MSR only for MB version < v7.20 */
	mts	rmsr, r9
#else
        nop
#endif

#else
	rtsd	r15, 8                                          /* Return */
	nop
#endif
	.end	microblaze_flush_dcache_range
