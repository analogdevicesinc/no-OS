/******************************************************************************
* Copyright (c) 2004 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
* @file microblaze_disable_interrupts.S
*
* @addtogroup microblaze_pseudo_asm_macro
* @{
* <h2> microblaze_disable_interrupts.S </h2>
*  - API to disable Interrupts: void microblaze_disable_interrupts(void)
*
* This API Disables interrupts on the MicroBlaze processor. It can be
* called when entering a critical section of code where a context switch is
* undesirable.
*
* <pre>
* File   : microblaze_disable_interrupts.s
* Date   : 2002, March 20.
* Company: Xilinx
* Group  : Emerging Software Technologies
*
* Summary:
* Disable interrupts on the microblaze.
* </pre>
*
******************************************************************************/

#include "xparameters.h"

	.text
	.globl	microblaze_disable_interrupts
	.ent	microblaze_disable_interrupts
	.align	2
microblaze_disable_interrupts:
#if XPAR_MICROBLAZE_USE_MSR_INSTR == 1
	rtsd	r15, 8
	msrclr  r0, 0x2
#else /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
	#Read the MSR register
	mfs	r12, rmsr
	#Clear the interrupt enable bit
	andi	r12, r12, ~(0x2)
	#Save the MSR register
	mts	rmsr, r12
	#Return
	rtsd	r15, 8
	nop
#endif /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
	.end	microblaze_disable_interrupts
/**
* @} End of "addtogroup microblaze_pseudo_asm_macro".
*/
