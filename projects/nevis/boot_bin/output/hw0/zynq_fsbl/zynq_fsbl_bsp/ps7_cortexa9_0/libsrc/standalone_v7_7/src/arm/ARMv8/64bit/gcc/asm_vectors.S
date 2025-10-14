/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
* @file asm_vectors.S
*
* This file contains the initial vector table for the Cortex A53 processor
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 5.00	pkp	05/21/14 Initial version
* 6.02  pkp	12/21/16 Added support for floating point access
* 6.02  pkp	01/22/17 Added support for EL1 non-secure and hypervisor
*			 baremetal guest
* 6.4   mus     06/14/17 Fixed bug in IRQInterruptHandler code snippet,
*                        which checks for the FPEN bit of CPACR_EL1
* 6.6   mus     01/19/18 Added isb after writing to the cpacr_el1/cptr_el3,
*                        to ensure enabling/disabling of floating-point unit
*                        is completed, before any subsequent instruction.
* 7.5   mus  05/20/21 Fixed speculative execution past ERET and BR instructions.
*                     As per CVE-2020-13844, Cortex-A72 is affected with
*                     vulnearability, hence changes are targeted only for Cortex-A72.
*                     It fixes CR#1083649.
* 7.7.  asa     03/22/22  Updated FIQ handler to also handle floating/SIMD context.
*
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

#include "bspconfig.h"

.org 0
.text

.globl _boot
.globl _vector_table

.globl FIQInterrupt
.globl IRQInterrupt
.globl SErrorInterrupt
.globl SynchronousInterrupt
.globl FPUStatus

/*
 * FPUContextSize is the size of the array where floating point registers are
 * stored when required. The default size corresponds to the case when there is no
 * nested interrupt. If there are nested interrupts in application which are using
 * floating point operation, the size of FPUContextSize need to be increased as per
 * requirement
 */

.set FPUContextSize, 528

.macro saveregister
	stp	X0,X1, [sp,#-0x10]!
	stp	X2,X3, [sp,#-0x10]!
	stp	X4,X5, [sp,#-0x10]!
	stp	X6,X7, [sp,#-0x10]!
	stp	X8,X9, [sp,#-0x10]!
	stp	X10,X11, [sp,#-0x10]!
	stp	X12,X13, [sp,#-0x10]!
	stp	X14,X15, [sp,#-0x10]!
	stp	X16,X17, [sp,#-0x10]!
	stp 	X18,X19, [sp,#-0x10]!
	stp 	X29,X30, [sp,#-0x10]!
.endm

.macro restoreregister
	ldp 	X29,X30, [sp], #0x10
	ldp 	X18,X19, [sp], #0x10
	ldp	X16,X17, [sp], #0x10
	ldp	X14,X15, [sp], #0x10
	ldp	X12,X13, [sp], #0x10
	ldp	X10,X11, [sp], #0x10
	ldp	X8,X9, [sp], #0x10
	ldp	X6,X7, [sp], #0x10
	ldp	X4,X5, [sp], #0x10
	ldp	X2,X3, [sp], #0x10
	ldp	X0,X1, [sp], #0x10
.endm

.macro savefloatregister

/* Load the floating point context array address from FPUContextBase */
	ldr	x1,=FPUContextBase
	ldr	x0, [x1]

/* Save all the floating point register to the array */
	stp	q0,q1, [x0], #0x20
	stp	q2,q3, [x0], #0x20
	stp	q4,q5, [x0], #0x20
	stp	q6,q7, [x0], #0x20
	stp	q8,q9, [x0], #0x20
	stp	q10,q11, [x0], #0x20
	stp	q12,q13, [x0], #0x20
	stp	q14,q15, [x0], #0x20
	stp	q16,q17, [x0], #0x20
	stp	q18,q19, [x0], #0x20
	stp	q20,q21, [x0], #0x20
	stp	q22,q23, [x0], #0x20
	stp	q24,q25, [x0], #0x20
	stp	q26,q27, [x0], #0x20
	stp	q28,q29, [x0], #0x20
	stp	q30,q31, [x0], #0x20
	mrs	x2, FPCR
	mrs	x3, FPSR
	stp	x2, x3, [x0], #0x10

/* Save current address of floating point context array to FPUContextBase */
	str	x0, [x1]
.endm

.macro restorefloatregister

/* Restore the address of floating point context array from FPUContextBase */
	ldr	x1,=FPUContextBase
	ldr	x0, [x1]

/* Restore all the floating point register from the array */
	ldp	x2, x3, [x0,#-0x10]!
	msr	FPCR, x2
	msr	FPSR, x3
	ldp	q30,q31, [x0,#-0x20]!
	ldp	q28,q29, [x0,#-0x20]!
	ldp	q26,q27, [x0,#-0x20]!
	ldp	q24,q25, [x0,#-0x20]!
	ldp	q22,q23, [x0,#-0x20]!
	ldp	q20,q21, [x0,#-0x20]!
	ldp	q18,q19, [x0,#-0x20]!
	ldp	q16,q17, [x0,#-0x20]!
	ldp	q14,q15, [x0,#-0x20]!
	ldp	q12,q13, [x0,#-0x20]!
	ldp	q10,q11, [x0,#-0x20]!
	ldp	q8,q9, [x0,#-0x20]!
	ldp	q6,q7, [x0,#-0x20]!
	ldp	q4,q5, [x0,#-0x20]!
	ldp	q2,q3, [x0,#-0x20]!
	ldp	q0,q1, [x0,#-0x20]!

/* Save current address of floating point context array to FPUContextBase */
	str	x0, [x1]
.endm

.macro exception_return
	eret
#if defined (versal)
	dsb nsh
	isb
#endif
.endm

.org 0

.section .vectors, "a"

_vector_table:
.set	VBAR, _vector_table
.org VBAR
/*
 * if application is built for XEN GUEST as EL1 Non-secure following image
 * header is required by XEN.
 */
.if (HYP_GUEST == 1)

	/* Valid Image header.  */
	/* HW reset vector.  */
	ldr	x16, =_boot
	br	x16
#if defined (versal)
	dsb nsh
	isb
#endif
	/* text offset.  */
	.dword	0
	/* image size.  */
	.dword	0
	/* flags.  */
	.dword	8
	/* RES0  */
	.dword	0
	.dword	0
	.dword	0

	/* magic  */
	.dword	0x644d5241
	/* RES0  */
	.dword	0
	/* End of Image header.  */
.endif

	b	_boot
.org (VBAR + 0x200)
	b	SynchronousInterruptHandler

.org (VBAR + 0x280)
	b	IRQInterruptHandler

.org (VBAR + 0x300)
	b	FIQInterruptHandler

.org (VBAR + 0x380)
	b	SErrorInterruptHandler


SynchronousInterruptHandler:
	saveregister

/* Check if the Synchronous abort is occurred due to floating point access. */
.if (EL3 == 1)
	mrs	x0, ESR_EL3
.else
	mrs	x0, ESR_EL1
.endif
	and	x0, x0, #(0x3F << 26)
	mov	x1, #(0x7 << 26)
	cmp	x0, x1
/* If exception is not due to floating point access go to synchronous handler */
	bne	synchronoushandler

/*
 * If excpetion occurred due to floating point access, Enable the floating point
 * access i.e. do not trap floating point instruction
 */
 .if (EL3 == 1)
	mrs	x1,CPTR_EL3
	bic	x1, x1, #(0x1<<10)
	msr	CPTR_EL3, x1
.else
	mrs	x1,CPACR_EL1
	orr	x1, x1, #(0x1<<20)
	msr	CPACR_EL1, x1
.endif
	isb

/* If the floating point access was previously enabled, store FPU context
 * registers(storefloat).
 */
	ldr	x0, =FPUStatus
	ldrb	w1,[x0]
	cbnz	w1, storefloat
/*
 * If the floating point access was not enabled previously, save the status of
 * floating point accessibility i.e. enabled and store floating point context
 * array address(FPUContext) to FPUContextBase.
 */
	mov	w1, #0x1
	strb	w1, [x0]
	ldr	x0, =FPUContext
	ldr	x1, =FPUContextBase
	str	x0,[x1]
	b	restorecontext
storefloat:
	savefloatregister
	b	restorecontext
synchronoushandler:
	bl	SynchronousInterrupt
restorecontext:
	restoreregister
	exception_return

IRQInterruptHandler:

	saveregister
/* Save the status of SPSR, ELR and CPTR to stack */
 .if (EL3 == 1)
	mrs 	x0, CPTR_EL3
	mrs 	x1, ELR_EL3
	mrs	x2, SPSR_EL3
.else
	mrs 	x0, CPACR_EL1
	mrs 	x1, ELR_EL1
	mrs	x2, SPSR_EL1
.endif
	stp	x0, x1, [sp,#-0x10]!
	str	x2, [sp,#-0x10]!

/* Trap floating point access */
 .if (EL3 == 1)
	mrs	x1,CPTR_EL3
	orr	x1, x1, #(0x1<<10)
	msr	CPTR_EL3, x1
.else
	mrs	x1,CPACR_EL1
	bic	x1, x1, #(0x1<<20)
	msr	CPACR_EL1, x1
.endif
	isb

	bl	IRQInterrupt
/*
 * If floating point access is enabled during interrupt handling,
 * restore floating point registers.
 */

 .if (EL3 == 1)
	mrs	x0, CPTR_EL3
	ands	x0, x0, #(0x1<<10)
	bne	RestorePrevState
.else
	mrs	x0,CPACR_EL1
	ands	x0, x0, #(0x1<<20)
	beq	RestorePrevState
.endif

	restorefloatregister

/* Restore the status of SPSR, ELR and CPTR from stack */
RestorePrevState:
	ldr	x2,[sp],0x10
	ldp	x0, x1, [sp],0x10
 .if (EL3 == 1)
	msr	CPTR_EL3, x0
	msr	ELR_EL3, x1
	msr	SPSR_EL3, x2
.else
	msr	CPACR_EL1, x0
	msr	ELR_EL1, x1
	msr	SPSR_EL1, x2
.endif
	restoreregister
	exception_return

FIQInterruptHandler:

  saveregister
/* Save the status of SPSR, ELR and CPTR to stack */
 .if (EL3 == 1)
	mrs 	x0, CPTR_EL3
	mrs 	x1, ELR_EL3
	mrs	x2, SPSR_EL3
.else
	mrs 	x0, CPACR_EL1
	mrs 	x1, ELR_EL1
	mrs	x2, SPSR_EL1
.endif
	stp	x0, x1, [sp,#-0x10]!
	str	x2, [sp,#-0x10]!

/* Trap floating point access */
 .if (EL3 == 1)
	mrs	x1,CPTR_EL3
	orr	x1, x1, #(0x1<<10)
	msr	CPTR_EL3, x1
.else
	mrs	x1,CPACR_EL1
	bic	x1, x1, #(0x1<<20)
	msr	CPACR_EL1, x1
.endif
	isb
	bl	FIQInterrupt
	/*
 * If floating point access is enabled during interrupt handling,
 * restore floating point registers.
 */

 .if (EL3 == 1)
	mrs	x0, CPTR_EL3
	ands	x0, x0, #(0x1<<10)
	bne	RestorePrevStatefiq
.else
	mrs	x0,CPACR_EL1
	ands	x0, x0, #(0x1<<20)
	beq	RestorePrevStatefiq
.endif

	restorefloatregister

	/* Restore the status of SPSR, ELR and CPTR from stack */
RestorePrevStatefiq:
	ldr	x2,[sp],0x10
	ldp	x0, x1, [sp],0x10
 .if (EL3 == 1)
	msr	CPTR_EL3, x0
	msr	ELR_EL3, x1
	msr	SPSR_EL3, x2
.else
	msr	CPACR_EL1, x0
	msr	ELR_EL1, x1
	msr	SPSR_EL1, x2
.endif
	restoreregister
	exception_return

SErrorInterruptHandler:

	saveregister

	bl      SErrorInterrupt

	restoreregister

	exception_return


.align 8
/* Array to store floating point registers */
FPUContext: .skip FPUContextSize

/* Stores address for floating point context array */
FPUContextBase: .skip 8

FPUStatus: .skip 1

.end
