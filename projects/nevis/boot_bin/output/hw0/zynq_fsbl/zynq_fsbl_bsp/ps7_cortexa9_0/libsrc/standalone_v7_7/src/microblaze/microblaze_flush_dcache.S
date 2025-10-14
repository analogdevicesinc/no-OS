/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
*
* microblaze_flush_dcache()
*
*    Flush the L1 DCache
*
*******************************************************************************/

#include "xparameters.h"
#include "microblaze_instructions.h"

#define MICROBLAZE_MSR_DCACHE_ENABLE        0x00000080
#define MICROBLAZE_MSR_INTR_ENABLE          0x00000002

#ifndef XPAR_MICROBLAZE_DCACHE_LINE_LEN
#define XPAR_MICROBLAZE_DCACHE_LINE_LEN   1
#endif

	.text
	.globl	microblaze_flush_dcache
	.ent	microblaze_flush_dcache
	.align	2

microblaze_flush_dcache:
#if (XPAR_MICROBLAZE_USE_DCACHE==1) && (XPAR_MICROBLAZE_ALLOW_DCACHE_WR==1)
	ADDIK 	r5, r0, XPAR_MICROBLAZE_DCACHE_BASEADDR & (-(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN))  /* Align to cache line */
	ADDIK	r6, r5, XPAR_MICROBLAZE_DCACHE_BYTE_SIZE & (-(4 * XPAR_MICROBLAZE_DCACHE_LINE_LEN))        /* Compute end */

L_start:
	wdc.flush r5, r0                                        /* Flush the Cache */

	CMPU	r18, r5, r6                                     /* Are we at the end? */
	BLEI	r18, L_done

#if defined (__arch64__ )
	addlik	r5, r5, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 */
        BRI L_start
#else
	brid	L_start                                         /* Branch to the beginning of the loop */
	addik	r5, r5, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 (delay slot) */
#endif
L_done:
#endif
	rtsd	r15, 8                                          /* Return */
        nop
	.end	microblaze_flush_dcache
