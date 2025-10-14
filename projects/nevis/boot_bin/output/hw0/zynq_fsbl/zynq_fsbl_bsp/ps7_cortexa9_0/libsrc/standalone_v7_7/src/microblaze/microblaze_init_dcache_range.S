/******************************************************************************
* Copyright (c) 2006 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* microblaze_init_dcache_range (unsigned int cache_start, unsigned int cache_len)
*
*    Invalidate dcache on the microblaze
*
*    Parameters:
*       'cache_start' - address in the Dcache where invalidation begins
*	'cache_len'   - length (in bytes) worth of Dcache to be invalidated
*
*
*******************************************************************************/

#include "xparameters.h"

#define MICROBLAZE_MSR_DCACHE_ENABLE        0x00000080
#define MICROBLAZE_MSR_INTR_ENABLE          0x00000002

#ifndef XPAR_MICROBLAZE_DCACHE_LINE_LEN
#define XPAR_MICROBLAZE_DCACHE_LINE_LEN   1
#endif

	.text
	.globl	microblaze_init_dcache_range
	.ent	microblaze_init_dcache_range
	.align	2

microblaze_init_dcache_range:
#if (XPAR_MICROBLAZE_USE_DCACHE==1) && (XPAR_MICROBLAZE_ALLOW_DCACHE_WR==1)
	mfs	r9, rmsr                                        /* Disable Dcache and interrupts before invalidating */
	andi	r10, r9, (~(MICROBLAZE_MSR_DCACHE_ENABLE | MICROBLAZE_MSR_INTR_ENABLE))
	mts	rmsr, r10

        andi    r5, r5, -(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN)  /* Align to cache line */

	add	r6, r5, r6                                      /* Compute end */
        andi    r6, r6, -(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN)  /* Align to cache line */

L_start:
	wdc	r5, r0                                          /* Invalidate the Cache (delay slot) */

	cmpu	r18, r5, r6                                     /* Are we at the end ? */
	blei	r18, L_done

	brid	L_start                                         /* Branch to the beginning of the loop */
	addik	r5, r5, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 (delay slot) */

L_done:
	rtsd	r15, 8                                          /* Return */
	mts	rmsr, r9
#else
	rtsd	r15, 8                                          /* Return */
	nop
#endif
	.end	microblaze_init_dcache_range
