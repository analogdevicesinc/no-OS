;******************************************************************************
; Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
; SPDX-License-Identifier: MIT
;*****************************************************************************
;****************************************************************************
;**
; @file asm_vectors.s
;
; This file contains the initial vector table for the Cortex A9 processor
;
; <pre>
; MODIFICATION HISTORY:
;
; Ver	Who     Date	  Changes
; ----- ------- -------- ---------------------------------------------------
; 1.00a ecm/sdm 10/20/09 Initial version
; 3.11a asa	9/17/13	 Added support for neon.
; 4.00  pkp	01/22/14 Modified return addresses for interrupt
;			 handlers
; 5.1	pkp	05/13/15 Saved the addresses of instruction causing data
;			 abort and prefetch abort into DataAbortAddr and
;			 PrefetchAbortAddr for further use to fix CR#854523
; 5.4	pkp	12/03/15 Added handler for undefined exception
;</pre>
;
; @note
;
; None.
;
;****************************************************************************

	EXPORT _vector_table
	EXPORT IRQHandler

	IMPORT _boot
	IMPORT _prestart
	IMPORT IRQInterrupt
	IMPORT FIQInterrupt
	IMPORT SWInterrupt
	IMPORT DataAbortInterrupt
	IMPORT PrefetchAbortInterrupt
	IMPORT UndefinedException
	IMPORT DataAbortAddr
	IMPORT PrefetchAbortAddr
	IMPORT UndefinedExceptionAddr

	AREA |.vectors|, CODE
	REQUIRE8     {TRUE}
	PRESERVE8    {TRUE}
	ENTRY ; define this as an entry point
_vector_table
	B	_boot
	B	Undefined
	B	SVCHandler
	B	PrefetchAbortHandler
	B	DataAbortHandler
	NOP	; Placeholder for address exception vector
	B	IRQHandler
	B	FIQHandler


IRQHandler					; IRQ vector handler

	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	vpush {d0-d7}
	vpush {d16-d31}
	vmrs r1, FPSCR
	push {r1}
	vmrs r1, FPEXC
	push {r1}
	bl	IRQInterrupt			; IRQ vector
	pop 	{r1}
	vmsr    FPEXC, r1
	pop 	{r1}
	vmsr    FPSCR, r1
	vpop    {d16-d31}
	vpop    {d0-d7}
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	subs	pc, lr, #4			; adjust return


FIQHandler					; FIQ vector handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	vpush {d0-d7}
	vpush {d16-d31}
	vmrs r1, FPSCR
	push {r1}
	vmrs r1, FPEXC
	push {r1}
FIQLoop
	bl	FIQInterrupt			; FIQ vector
	pop 	{r1}
	vmsr    FPEXC, r1
	pop 	{r1}
	vmsr    FPSCR, r1
	vpop    {d16-d31}
	vpop    {d0-d7}
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	subs	pc, lr, #4			; adjust return


Undefined					; Undefined handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	ldr     r0, =UndefinedExceptionAddr
	sub     r1, lr,#4
	str     r1, [r0]			; Address of instruction causing undefined exception
	bl	UndefinedException		; UndefinedException: call C function here
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code

	movs	pc, lr


SVCHandler					; SWI handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	tst	r0, #0x20			; check the T bit
	ldrneh	r0, [lr,#-2]			; Thumb mode
	bicne	r0, r0, #0xff00			; Thumb mode
	ldreq	r0, [lr,#-4]			; ARM mode
	biceq	r0, r0, #0xff000000		; ARM mode
	bl	SWInterrupt			; SWInterrupt: call C function here
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	movs	pc, lr				; adjust return

DataAbortHandler				; Data Abort handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	ldr     r0, =DataAbortAddr
	sub     r1, lr,#8
	str     r1, [r0]			;Address of instruction causing data abort
	bl	DataAbortInterrupt		;DataAbortInterrupt :call C function here
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	subs	pc, lr, #8			; adjust return

PrefetchAbortHandler				; Prefetch Abort handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	ldr     r0, =PrefetchAbortAddr
	sub     r1, lr,#4
	str     r1, [r0]			;Address of instruction causing prefetch abort
	bl	PrefetchAbortInterrupt		; PrefetchAbortInterrupt: call C function here
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	subs	pc, lr, #4			; adjust return

	END
