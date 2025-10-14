/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* microblaze_invalidate_dcache()
*
*    Invalidate the entire L1 DCache
*
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
#endif

	.text
	.globl	microblaze_invalidate_dcache
	.ent	microblaze_invalidate_dcache
	.align	2

microblaze_invalidate_dcache:
#if (XPAR_MICROBLAZE_USE_DCACHE==1) && (XPAR_MICROBLAZE_ALLOW_DCACHE_WR==1)

#ifdef MB_VERSION_LT_v720                                       /* Disable Dcache and interrupts before invalidating */
	mfs	r9, rmsr
	andi	r10, r9, ~(MICROBLAZE_MSR_DCACHE_ENABLE | MICROBLAZE_MSR_INTR_ENABLE)
	mts	rmsr, r10
#endif
	ADDIK 	r5, r0, XPAR_MICROBLAZE_DCACHE_BASEADDR & (-(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN))
	ADDIK	r6, r5, XPAR_MICROBLAZE_DCACHE_BYTE_SIZE &  (-(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN))        /* Compute end */

L_start:
	wdc	r5, r0                                          /* Invalidate the Cache */

	CMPU	r18, r5, r6                                     /* Are we at the end? */
	BLEI	r18, L_done

#if defined (__arch64__ )
	addlik	r5, r5, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 */
        breai	L_start
#else
	brid	L_start                                         /* Branch to the beginning of the loop */
	addik	r5, r5, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 (delay slot) */
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
	.end	microblaze_invalidate_dcache
