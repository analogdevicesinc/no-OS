/******************************************************************************
* Copyright (c) 2004 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
* @file microblaze_enable_interrupts.S
*
* @addtogroup microblaze_pseudo_asm_macro
* @{
* <h2> microblaze_enable_interrupts.S </h2>
*  - API to Enable Interrupts: void microblaze_enable_interrupts(void)
*
* This API Enables interrupts on the MicroBlaze processor. When the MicroBlaze
* processor starts up, interrupts are disabled. Interrupts must be explicitly
* turned on using this function.
*
* <pre>
*
* File   : microblaze_enable_interrupts.s
* Date   : 2002, March 20.
* Company: Xilinx
* Group  : Emerging Software Technologies
*
* Summary:
* Enable interrupts on the microblaze.
* </pre>
*
******************************************************************************/

#include "xparameters.h"

	.text
	.globl	microblaze_enable_interrupts
	.ent	microblaze_enable_interrupts
	.align	2
microblaze_enable_interrupts:
#if XPAR_MICROBLAZE_USE_MSR_INSTR == 1
	rtsd	r15, 8
	msrset  r0, 0x2
	nop
#else /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
	#Read the MSR register
	mfs	r12, rmsr
	#Set the interrupt enable bit
	ori	r12, r12, 0x2
	#Save the MSR register
	mts	rmsr, r12
	#Return
	rtsd	r15, 8
	nop
#endif /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
	.end	microblaze_enable_interrupts
/**
* @} End of "addtogroup microblaze_pseudo_asm_macro".
*/
