/******************************************************************************
* Copyright (c) 2004 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* File   : microblaze_disable_icache.s
* Date   : 2002, March 20.
* Company: Xilinx
* Group  : Emerging Software Technologies
*
* Summary:
* Disable L1 icache on the microblaze.
*
*
******************************************************************************/

#include "xparameters.h"

	.text
	.globl	microblaze_disable_icache
	.ent	microblaze_disable_icache
	.align	2
microblaze_disable_icache:
#if XPAR_MICROBLAZE_USE_MSR_INSTR == 1
	rtsd	r15, 8
	msrclr  r0, 0x20
#else /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
	#Read the MSR register
	mfs	r8, rmsr
	#Clear the icache enable bit
	andi	r8, r8, ~(0x20)
	#Save the MSR register
	mts	rmsr, r8
	#Return
	rtsd	r15, 8
	nop
#endif
	.end	microblaze_disable_icache
