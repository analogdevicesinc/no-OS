/******************************************************************************
* Copyright (c) 2015 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*****************************************************************************/
/**
* @file boot.S
*
* @addtogroup a53_32_boot_code Cortex A53 32bit Processor Boot Code
* @{
* <h2> boot.S </h2>
* The boot.S file contains a minimal set of code for transferring control from the
* processor reset location to the start of the application. The boot code performs
* minimum configuration which is required for an application to run starting from
* processor reset state of the processor. Below is a sequence illustrating what all
* configuration is performed before control reaches to main function.
*
* 1. Program vector table base for exception handling
* 2. Invalidate instruction cache, data cache and TLBs
* 3. Program stack pointer for various modes (IRQ, FIQ, supervisor, undefine,
*    abort, system)
* 4. Program counter frequency
* 5. Configure MMU with short descriptor translation table format and program
*    base address of translation table
* 6. Enable data cache, instruction cache and MMU
* 7. Transfer control to _start which clears BSS sections and runs global
*    constructor before jumping to main application
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------- -------- ---------------------------------------------------
* 5.2	pkp  	28/05/15 First release
* 5.4	pkp	09/11/15 Enable I-Cache and D-Cache in the initialization
* 6.0	pkp	07/25/16 Program the counter frequency
* 6.4   mus     07/25/17 Set VFP enable bit in FPEXC register, to support
*                        hard floating point in BSP
* </pre>
*
*
******************************************************************************/

#include "xparameters.h"


.globl MMUTable
.global _prestart
.global _boot
.global __stack
.global __irq_stack
.global __supervisor_stack
.global __abort_stack
.global __fiq_stack
.global __undef_stack
.global _vector_table

.set PSS_L2CC_BASE_ADDR, 0xF8F02000
.set PSS_SLCR_BASE_ADDR, 0xF8000000

.set RESERVED,		0x0fffff00
.set TblBase ,		MMUTable
.set LRemap,		0xFE00000F		/* set the base address of the peripheral block as not shared */

.set CRValMmuCac,	0b01000000000001	/* Enable IDC, and MMU */
.set counterfreq,	XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ

/* Stack Pointer locations for boot code */
.set Undef_stack,	__undef_stack
.set FIQ_stack,		__fiq_stack
.set Abort_stack,	__abort_stack
.set SPV_stack,		__supervisor_stack
.set IRQ_stack,		__irq_stack
.set SYS_stack,		__stack

.set vector_base,	_vector_table

.set FPEXC_EN,		0x40000000		/* FPU enable bit, (1 << 30) */

.section .boot,"ax"

/* this initializes the various processor modes */

_prestart:
_boot:

OKToRun:
	mrc     p15, 0, r0, c0, c0, 0		/* Get the revision */
	and     r5, r0, #0x00f00000
	and     r6, r0, #0x0000000f
	orr     r6, r6, r5, lsr #20-4

	/* set VBAR to the _vector_table address in linker script */
	ldr	r0, =vector_base
	mcr	p15, 0, r0, c12, c0, 0

	/* Invalidate caches and TLBs */
	mov	r0,#0				/* r0 = 0  */
	mcr	p15, 0, r0, c8, c7, 0		/* invalidate TLBs */
	mcr	p15, 0, r0, c7, c5, 0		/* invalidate icache */
	mcr	p15, 0, r0, c7, c5, 6		/* Invalidate branch predictor array */
	bl	invalidate_dcache		/* invalidate dcache */

	/* Disable MMU, if enabled */
	mrc	p15, 0, r0, c1, c0, 0		/* read CP15 register 1 */
	bic	r0, r0, #0x1			/* clear bit 0 */
	mcr	p15, 0, r0, c1, c0, 0		/* write value back */


	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the irq stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x12			/* IRQ mode */
	msr	cpsr, r2
	ldr	r13,=IRQ_stack			/* IRQ stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the supervisor stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x13			/* supervisor mode */
	msr	cpsr, r2
	ldr	r13,=SPV_stack			/* Supervisor stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the Abort  stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x17			/* Abort mode */
	msr	cpsr, r2
	ldr	r13,=Abort_stack		/* Abort stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the FIQ stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x11			/* FIQ mode */
	msr	cpsr, r2
	ldr	r13,=FIQ_stack			/* FIQ stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the Undefine stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x1b			/* Undefine mode */
	msr	cpsr, r2
	ldr	r13,=Undef_stack		/* Undefine stack pointer */

	mrs	r0, cpsr			/* get the current PSR */
	mvn	r1, #0x1f			/* set up the system stack pointer */
	and	r2, r1, r0
	orr	r2, r2, #0x1F			/* SYS mode */
	msr	cpsr, r2
	ldr	r13,=SYS_stack			/* SYS stack pointer */

	/* program the timer counter frequency */
	ldr	r0,=counterfreq
	mcr	15,0,r0,c14,c0,0

	mov	r0,#0
	mcr 	15,0,r0,c2,c0,2			/* N = 0 to use ttbr0 */

        /* Write to ACTLR */
	mrc p15, 0, r0, c1, c0, 1 /* Read ACTLR */
	orr r0, r0, #(1 << 0)     /* Enable access to CPUECTLR */
	orr r0, r0, #(1 << 1)
	mcr p15, 0, r0, c1, c0, 1 /* Write ACTLR */

       /* Write to CPUECTLR */
	mrrc p15, 1, r0, r1, c15  /* Read CPUECTLR */
	orr r0, r0, #(0x01 << 6)  /* Set SMPEN bit */
	mcrr p15, 1, r0, r1, c15  /* Write CPUECTLR */

	/* enable MMU and cache */
	ldr	r0,=TblBase			/* Load MMU translation table base */
	orr	r0, r0, #0x5B			/* Outer-cacheable, WB */
	mcr	15, 0, r0, c2, c0, 0		/* TTB0 */
	mov	r0,#0x5B
	mcr	p15,0,r0,c2,c0,1
	mvn	r0,#0				/* Load MMU domains -- all ones=manager */
	mcr	p15,0,r0,c3,c0,0

	/* Enable mmu, icahce and dcache */
	mrc p15,0,r0,c1,c0,0
	bic r0, r0, #(0x1 << 13)
	orr r0, r0, #(0x1 << 12)		/* enable I-cache */
	orr r0, r0, #(0x1 << 2)			/* enable D-Cache */
	orr r0, r0, #0x1			/* enable MMU */
	dsb					/* dsb	allow the MMU to start up */
	mcr	p15,0,r0,c1,c0,0		/* Enable cache and MMU */
	isb					/* isb	flush prefetch buffer */

	mov	r0, r0
	mrc	p15, 0, r1, c1, c0, 2		/* read cp access control register (CACR) into r1 */
	orr	r1, r1, #(0xf << 20)		/* enable full access for p10 & p11 */
	mcr	p15, 0, r1, c1, c0, 2		/* write back into CACR */

	/* enable vfp */
	vmrs	r1, FPEXC			/* read the exception register */
	orr	r1,r1, #FPEXC_EN		/* set VFP enable bit, leave the others in orig state */
	vmsr	FPEXC, r1			/* write back the exception register */

	mrc	p15,0,r0,c1,c0,0		/* flow prediction enable */
	orr	r0, r0, #(0x01 << 11)		/* #0x8000 */
	mcr	p15,0,r0,c1,c0,0

	mrc	p15,0,r0,c1,c0,1		/* read Auxiliary Control Register */
	orr	r0, r0, #(0x1 << 2)		/* enable Dside prefetch */
	orr	r0, r0, #(0x1 << 1)		/* enable L2 Prefetch hint */
	mcr	p15,0,r0,c1,c0,1		/* write Auxiliary Control Register */

	mrs	r0, cpsr			/* get the current PSR */
	bic	r0, r0, #0x100			/* enable asynchronous abort exception */
	msr	cpsr_xsf, r0


	b	_startup				/* jump to C startup code */
	and	r0, r0, r0			/* no op */

.Ldone:	b	.Ldone				/* Paranoia: we should never get here */


/*
 *************************************************************************
 *
 * invalidate_dcache - invalidate the entire d-cache by set/way
 *
 * Note: for Cortex-A53, there is no cp instruction for invalidating
 * the whole D-cache. Need to invalidate each line.
 *
 *************************************************************************
 */
invalidate_dcache:
	mrc	p15, 1, r0, c0, c0, 1		/* read CLIDR */
	ands	r3, r0, #0x7000000
	mov	r3, r3, lsr #23			/* cache level value (naturally aligned) */
	beq	finished
	mov	r10, #0				/* start with level 0 */
loop1:
	add	r2, r10, r10, lsr #1		/* work out 3xcachelevel */
	mov	r1, r0, lsr r2			/* bottom 3 bits are the Cache type for this level */
	and	r1, r1, #7			/* get those 3 bits alone */
	cmp	r1, #2
	blt	skip				/* no cache or only instruction cache at this level */
	mcr	p15, 2, r10, c0, c0, 0		/* write the Cache Size selection register */
	isb					/* isb to sync the change to the CacheSizeID reg */
	mrc	p15, 1, r1, c0, c0, 0		/* reads current Cache Size ID register */
	and	r2, r1, #7			/* extract the line length field */
	add	r2, r2, #4			/* add 4 for the line length offset (log2 16 bytes) */
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		/* r4 is the max number on the way size (right aligned) */
	clz	r5, r4				/* r5 is the bit position of the way size increment */
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		/* r7 is the max number of the index size (right aligned) */
loop2:
	mov	r9, r4				/* r9 working copy of the max way size (right aligned) */
loop3:
	orr	r11, r10, r9, lsl r5		/* factor in the way number and cache number into r11 */
	orr	r11, r11, r7, lsl r2		/* factor in the index number */
	mcr	p15, 0, r11, c7, c6, 2		/* invalidate by set/way */
	subs	r9, r9, #1			/* decrement the way number */
	bge	loop3
	subs	r7, r7, #1			/* decrement the index */
	bge	loop2
skip:
	add	r10, r10, #2			/* increment the cache number */
	cmp	r3, r10
	bgt	loop1

finished:
	mov	r10, #0				/* switch back to cache level 0 */
	mcr	p15, 2, r10, c0, c0, 0		/* select current cache level in cssr */
	dsb
	isb

	bx	lr

.end
/**
* @} End of "addtogroup a53_32_boot_code".
*/
