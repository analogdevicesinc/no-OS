/******************************************************************************
* Copyright (C) 2014 - 2022 Xilinx, Inc.  All rights reserved.
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
* 5.00	pkp  05/21/14 Initial version
* 5.04  pkp  12/18/15 Initialized global constructor for C++ applications
* 5.04	pkp  01/05/16 Set the reset vector register RVBAR equivalent to
*		      vector table base address
* 6.02  pkp  01/22/17 Added support for EL1 non-secure
* 6.6   srm  10/18/17 Added timer configuration using XTime_StartTTCTimer API.
*		      Now the TTC instance as specified by the user will be
*	              started.
* 6.6   mus  01/29/18 Initialized the xen PV console for Cortexa53 64 bit
*                     EL1 NS BSP.
* 7.2   sd   02/23/20 Clock Init is called
* 7.2   sd   02/23/20 Clock code added under XCLOCKING flag
* 7.7   mus  01/06/22 Added call to Xil_SetTlbAttributes to set correct
*                     attributes for GIC in case of Xen domU guest application.
*                     It fixes CR#974078.
* </pre>
*
* @note
*
* None.
*
******************************************************************************/
#include "xparameters.h"
#include "bspconfig.h"
	.file	"xil-crt0.S"
	.section ".got2","aw"
	.align	2

	.text
.Lsbss_start:
	.quad	__sbss_start

.Lsbss_end:
	.quad	__sbss_end

.Lbss_start:
	.quad	__bss_start__

.Lbss_end:
	.quad	__bss_end__


.set APU_PWRCTL,	0xFD5C0090

	.globl	_startup
_startup:

	mov	x0, #0
.if (EL3 == 1)
	/* Check whether the clearing of bss sections shall be skipped */
	ldr	x10, =APU_PWRCTL	/* Load PWRCTRL address */
	ldr	w11, [x10]		/* Read PWRCTRL register */
	mrs	x2, MPIDR_EL1		/* Read MPIDR_EL1 */
	ubfx	x2, x2, #0, #8		/* Extract CPU ID (affinity level 0) */
	mov	w1, #1
	lsl	w2, w1, w2		/* Shift CPU ID to get one-hot ID */
	ands	w11, w11, w2		/* Get PWRCTRL bit for this core */
	bne	.Lenclbss		/* Skip BSS and SBSS clearing */
.endif
	/* clear sbss */
	ldr 	x1,.Lsbss_start		/* calculate beginning of the SBSS */
	ldr	x2,.Lsbss_end		/* calculate end of the SBSS */

.Lloop_sbss:
	cmp	x1,x2
	bge	.Lenclsbss		/* If no SBSS, no clearing required */
	str	x0, [x1], #8
	b	.Lloop_sbss

.Lenclsbss:
	/* clear bss */
	ldr	x1,.Lbss_start		/* calculate beginning of the BSS */
	ldr	x2,.Lbss_end		/* calculate end of the BSS */

.Lloop_bss:
	cmp	x1,x2
	bge	.Lenclbss		/* If no BSS, no clearing required */
	str	x0, [x1], #8
	b	.Lloop_bss

.Lenclbss:
	/* run global constructors */
	bl __libc_init_array

	/* Reset and start Triple Timer Counter */
	#if defined (SLEEP_TIMER_BASEADDR)
	bl XTime_StartTTCTimer
	#endif

	.if (EL1_NONSECURE == 1 && HYP_GUEST == 1)
	/*
	 * Xen domU guest memory map is not same as that of
	 * native ZynqMP memory map. Currently GIC for Xen
         * domU guest is being mapped at < 2GB address, which
	 * is configured as normal cacheable memory (DDR) in
	 * default translation table. As GIC needs to be
	 * configured as device memory, updating attributes
	 * of GIC region as strongly ordered, RW, non executable
	 * through Xil_SetTlbAttributes API. Below code snippet
	 * in assemby is equivalent to,
	 * Xil_SetTlbAttributes(XPAR_SCUGIC_0_DIST_BASEADDR,
	 * 			STRONG_ORDERED | EXECUTE_NEVER)
	 */
	 ldr    x0, =XPAR_SCUGIC_0_DIST_BASEADDR
         mov    x1, #0x409
         orr    x1, x1, #(0x1 << 53)
         orr    x1, x1, #(0x1 << 54)
         bl     Xil_SetTlbAttributes
	.endif

	.if (EL1_NONSECURE == 1 && HYP_GUEST == 1 && \
	     XEN_USE_PV_CONSOLE == 1)
         bl XPVXenConsole_Init
	.endif
	/* make sure argc and argv are valid */
	mov	x0, #0
	mov	x1, #0

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
