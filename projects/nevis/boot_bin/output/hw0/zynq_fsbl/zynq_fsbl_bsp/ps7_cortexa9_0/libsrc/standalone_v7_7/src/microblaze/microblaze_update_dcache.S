/******************************************************************************
* Copyright (c) 2006 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
*
* File   : microblaze_update_dcache.s
* Date   : 2003, September 24
* Company: Xilinx
* Group  : Emerging Software Technologies
*
* Summary:
* Update dcache on the microblaze.
* Takes in three parameters
*	r5 : Cache Tag Line
*	r6 : Cache Data
*	r7 : Lock/Valid information
*		Bit 30 is Lock  [ 1 indicates locked ]
*		Bit 31 is Valid [ 1 indicates valid ]
*
*	--------------------------------------------------------------
*	|  Lock	 |     Valid  | Effect
*	--------------------------------------------------------------
*	|   0    |      0     | Invalidate Cache
*       |   0    |      1     | Valid, but unlocked cacheline
*       |   1    |      0     | Invalidate Cache, No effect of lock
*       |   1    |      1     | Valid cache. Locked to a
*       |        |            | particular address
*	--------------------------------------------------------------
*
*
**********************************************************************************/
#include "xparameters.h"

#ifndef XPAR_MICROBLAZE_DCACHE_LINE_LEN
#define XPAR_MICROBLAZE_DCACHE_LINE_LEN   1
#endif

	.text
	.globl	microblaze_update_dcache
	.ent	microblaze_update_dcache
	.align	2
microblaze_update_dcache:
#if (XPAR_MICROBLAZE_USE_DCACHE==1) && (XPAR_MICROBLAZE_ALLOW_DCACHE_WR==1)

#if XPAR_MICROBLAZE_DCACHE_LINE_LEN == 1

/* Read the MSR register into a temp register */
	mfs	r18, rmsr

/* Clear the dcache enable bit to disable the cache
   Register r10,r18 are  volatile registers and hence do not need to be saved before use */
	andi	r10, r18, ~128
	mts	rmsr, r10

/* Update the lock and valid info */
	andi	r5, r5, 0xfffffffc
	or	r5, r5, r7

/* Update dcache */
	wdc	r5, r6

/* Return */
	rtsd	r15, 8
	mts	rmsr, r18

#else

        /* The only valid usage of this routine for larger cache line lengths is to invalidate a data cache line
           So call microblaze_init_dcache_range appropriately to do the job   */

        brid    microblaze_init_dcache_range
        addik   r6, r0, (XPAR_MICROBLAZE_DCACHE_LINE_LEN * 4)

        /* We don't have a return instruction here. This is tail call optimization :) */

#endif /* XPAR_MICROBLAZE_DCACHE_LINE_LEN == 1 */

#else
	rtsd	r15, 8
	nop
#endif
	.end	microblaze_update_dcache
