;******************************************************************************
; Copyright (c) 2017 - 2020 Xilinx, Inc.  All rights reserved.
; SPDX-License-Identifier: MIT
;*****************************************************************************/
;*****************************************************************************/
;**
; @file asm_vectors.s
;
; This file contains the initial vector table for the Cortex R5 processor
;
; <pre>
; MODIFICATION HISTORY:
;
; Ver   Who     Date     Changes
; ----- ------- -------- ---------------------------------------------------
; 6.2   mus	01/27/17 Initial version
; </pre>
;
; @note
;
; None.
;
;*****************************************************************************/

        MODULE  ?asm_vectors

        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION SVC_STACK:DATA:NOROOT(3)
        SECTION ABT_STACK:DATA:NOROOT(3)
        SECTION UND_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)

#define UART_BAUDRATE	115200

	IMPORT _prestart
	IMPORT __iar_program_start

       SECTION .intvec:CODE:NOROOT(2)



	PUBLIC _vector_table

	IMPORT FIQInterrupt
	IMPORT IRQInterrupt
	IMPORT SWInterrupt
	IMPORT DataAbortInterrupt
	IMPORT PrefetchAbortInterrupt
        IMPORT UndefinedException
        IMPORT UndefinedExceptionAddr
        IMPORT PrefetchAbortAddr
        IMPORT DataAbortAddr

	IMPORT prof_pc


_vector_table
        ARM
	ldr	pc,=__iar_program_start
	ldr	pc,=Undefined
	ldr	pc,=SVCHandler
	ldr	pc,=PrefetchAbortHandler
	ldr	pc,=DataAbortHandler
	NOP	; Placeholder for address exception vector
	ldr	pc,=IRQHandler
	ldr	pc,=FIQHandler

	SECTION .text:CODE:NOROOT(2)
        REQUIRE _vector_table

        ARM
IRQHandler					; IRQ vector handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
#ifndef __SOFTFP__
	vpush {d0-d7}				/* Store floating point registers */
	vmrs r1, FPSCR
	push {r1}
	vmrs r1, FPEXC
	push {r1}
#endif
	bl	IRQInterrupt			; IRQ vector
#ifndef __SOFTFP__
	pop 	{r1}				/* Restore floating point registers */
	vmsr    FPEXC, r1
	pop 	{r1}
	vmsr    FPSCR, r1
	vpop    {d0-d7}
#endif
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
	ldr     r0, =UndefinedExceptionAddr
	sub     r1, lr, #4
	str     r1, [r0]            		; Store address of instruction causing undefined exception

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
	sub     r1, lr, #8
	str     r1, [r0]            		; Stores instruction causing data abort
	bl	DataAbortInterrupt		;DataAbortInterrupt :call C function here
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code
	subs	pc, lr, #8			; adjust return

PrefetchAbortHandler				; Prefetch Abort handler
	stmdb	sp!,{r0-r3,r12,lr}		; state save from compiled code
	ldr     r0, =PrefetchAbortAddr
	sub     r1, lr, #4
	str     r1, [r0]            		; Stores instruction causing prefetch abort */
	bl	PrefetchAbortInterrupt		; PrefetchAbortInterrupt: call C function here */
	ldmia	sp!,{r0-r3,r12,lr}		; state restore from compiled code */
	subs	pc, lr, #4			; adjust return */


        END
