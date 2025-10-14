/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
* @file xil-crt0.S
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 5.00 	pkp  02/10/14 First release
* 5.04  pkp  12/18/15 Initialized global constructor for C++ applications
* 5.04  pkp  02/19/16 Added timer configuration using XTime_StartTimer API when
*		      TTC3 is present
* 6.4   asa  08/16/17 Added call to Xil_InitializeExistingMPURegConfig to
*                     initialize the MPU configuration table with the MPU
*                     configurations already set in Init_Mpu function.
* 6.6   srm  10/18/17 Updated the timer configuration with XTime_StartTTCTimer.
*                     Now the timer instance as specified by the user will be
*		      started.
* 7.2   mus  10/22/19 Defined RPU_TZ_MASK as #define instead of variable.
* 7.2   sd   03/20/20 Add clocking support.
* 7.7   adk  11/30/21 Added support for xiltimer library.
* </pre>
*
******************************************************************************/
#include "xparameters.h"
#include "bspconfig.h"
	.file	"xil-crt0.S"
	.section ".got2","aw"
	.align	2
/*
 * 0th bit of PROCESSOR_ACCESS_VALUE macro signifies trustzone
 * setting for RPU address space
 */
#define RPU_TZ_MASK	0x1
	.text
.Lsbss_start:
	.long	__sbss_start

.Lsbss_end:
	.long	__sbss_end

.Lbss_start:
	.long	__bss_start__

.Lbss_end:
	.long	__bss_end__

.Lstack:
	.long	__stack

.set RPU_0_PWRCTL,	0xFF9A0108
.set RPU_1_PWRCTL,	0xFF9A0208
.set MPIDR_AFF0,	0xFF
.set PWRCTL_MASK,	0x1

	.globl	_startup

_startup:
	bl      __cpu_init		/* Initialize the CPU first (BSP provides this) */
#if (PROCESSOR_ACCESS_VALUE & RPU_TZ_MASK)
	mrc	p15, 0, r0,  c0,  c0, 5	/* Read MPIDR register */
	ands	r0, r0, #MPIDR_AFF0	/* Get affinity level 0 */
	bne	core1
	ldr	r10, =RPU_0_PWRCTL	/* Load PWRCTRL address for core 0 */
	b	test_boot_status

core1:
	ldr	r10, =RPU_1_PWRCTL	/* Load PWRCTRL address for core 1 */

test_boot_status:
	ldr	r11, [r10]		/* Read PWRCTRL register */
	ands	r11, r11, #PWRCTL_MASK	/* Extract and test core's PWRCTRL */

	/* if warm reset, skip the clearing of BSS and SBSS */
	bne	.Lenclbss
#endif
	mov	r0, #0

	/* clear sbss */
	ldr 	r1,.Lsbss_start		/* calculate beginning of the SBSS */
	ldr	r2,.Lsbss_end		/* calculate end of the SBSS */

.Lloop_sbss:
	cmp	r1,r2
	bge	.Lenclsbss		/* If no SBSS, no clearing required */
	str	r0, [r1], #4
	b	.Lloop_sbss

.Lenclsbss:
	/* clear bss */
	ldr	r1,.Lbss_start		/* calculate beginning of the BSS */
	ldr	r2,.Lbss_end		/* calculate end of the BSS */

.Lloop_bss:
	cmp	r1,r2
	bge	.Lenclbss		/* If no BSS, no clearing required */
	str	r0, [r1], #4
	b	.Lloop_bss

.Lenclbss:

	/* set stack pointer */
	ldr	r13,.Lstack		/* stack address */

	/* configure the timer if TTC is present */
#ifndef XPAR_XILTIMER_ENABLED
#ifdef SLEEP_TIMER_BASEADDR
    bl XTime_StartTTCTimer
#endif
#endif
	bl 	Xil_InitializeExistingMPURegConfig	/* Initialize MPU config */
	/* run global constructors */
	bl __libc_init_array

	/* make sure argc and argv are valid */
	mov	r0, #0
	mov	r1, #0

#ifdef XCLOCKING
	bl	Xil_ClockInit
#endif
	bl	main			/* Jump to main C code */

	/* Cleanup global constructors */
	bl __libc_fini_array

	bl	exit

.Lexit:	/* should never get here */
	b .Lexit

.Lstart:
	.size	_startup,.Lstart-_startup
