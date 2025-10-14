/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
* File   : microblaze_disable_dcache.s
* Date   : 2002, March 20.
* Company: Xilinx
* Group  : Emerging Software Technologies
*
* Summary:
* Disable the L1 dcache on the microblaze.
*
*******************************************************************************/

#include "xparameters.h"
#include "microblaze_instructions.h"

	.text
	.globl	microblaze_disable_dcache
	.ent	microblaze_disable_dcache
	.align	2
microblaze_disable_dcache:
#if XPAR_MICROBLAZE_USE_MSR_INSTR == 1

#if XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK != 0
    ADDIK   r1, r1, -8
    SI     r15, r1, 0
    BRLID   r15, microblaze_flush_dcache /* microblaze_flush_dcache does not use r1*/
    nop
    LI     r15, r1, 0
    ADDIK    r1, r1, 8
#endif /* XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK != 0 */
	rtsd	r15, 8
	msrclr r0, 0x80

#else /* XPAR_MICROBLAZE_USE_MSR_INSTR == 1 */

    ADDIK   r1, r1, -8

#if XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK != 0
    SI     r15, r1, 0
    BRLID   r15, microblaze_flush_dcache
    nop
#endif /* XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK != 0 */
    mfs	r11, rmsr
	andi	r11, r11, ~(0x80)
	mts	rmsr, r11

#if XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK != 0
    LI     r15, r1, 0
#endif /* XPAR_MICROBLAZE_DCACHE_USE_WRITEBACK != 0 */

	ADDIK	r1, r1, 8
	rtsd	r15, 8
	nop

#endif /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
        .end	microblaze_disable_dcache
