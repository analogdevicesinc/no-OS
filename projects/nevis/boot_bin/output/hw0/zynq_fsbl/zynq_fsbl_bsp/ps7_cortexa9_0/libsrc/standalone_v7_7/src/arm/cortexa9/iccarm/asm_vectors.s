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
; 1.00a 		 Initial version
; 4.2 	pkp 	06/27/14 Modified return addresses for interrupt
;			 handlers
; 5.1	pkp	05/13/15 Saved the addresses of instruction causing data
;			 abort and prefetch abort into DataAbortAddr and
;			 PrefetchAbortAddr for further use to fix CR#854523
; </pre>
;
; @note
;
; None.
;
;****************************************************************************

        MODULE  ?asm_vectors

        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION SVC_STACK:DATA:NOROOT(3)
        SECTION ABT_STACK:DATA:NOROOT(3)
        SECTION UND_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)

#include "xparameters.h"
;#include "xtime_l.h"

#define UART_BAUDRATE	115200

	IMPORT _prestart
	IMPORT __iar_program_start



        SECTION .intvec:CODE:NOROOT(2)

        PUBLIC _vector_table

	IMPORT IRQInterrupt
	IMPORT FIQInterrupt
	IMPORT SWInterrupt
	IMPORT DataAbortInterrupt
	IMPORT PrefetchAbortInterrupt
	IMPORT DataAbortAddr
	IMPORT PrefetchAbortAddr

_vector_table
        ARM

	B	__iar_program_start
	B	Undefined
	B	SVCHandler
	B	PrefetchAbortHandler
	B	DataAbortHandler
	NOP	; Placeholder for address exception vector
	B	IRQHandler
	B	FIQHandler


      SECTION .text:CODE:NOROOT(2)
      REQUIRE _vector_table

        ARM
IRQHandler					; IRQ vector handler

	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	bl	IRQInterrupt			; IRQ vector
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	subs	pc, lr, #4			; adjust return


FIQHandler					; FIQ vector handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code

FIQLoop
	bl	FIQInterrupt			; FIQ vector

	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	subs	pc, lr, #4			; adjust return


Undefined					; Undefined handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	b	_prestart

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
