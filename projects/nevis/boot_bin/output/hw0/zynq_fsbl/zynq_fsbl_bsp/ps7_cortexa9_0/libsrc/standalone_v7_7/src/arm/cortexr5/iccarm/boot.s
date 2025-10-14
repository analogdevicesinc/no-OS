;******************************************************************************
; Copyright (c) 2014 - 2020 Xilinx, Inc.  All rights reserved.
; SPDX-License-Identifier: MIT
; *****************************************************************************/
; ****************************************************************************/
; **
;  @file boot.S
;
;  This file contains the initial startup code for the Cortex R5 processor
;
;  <pre>
;  MODIFICATION HISTORY:
;
;  Ver   Who  Date     Changes
;  ----- ---- -------- ---------------------------------------------------
; 5.00  mus  01/27/17 Initial version
; 6.6   srm  10/18/17 Updated the timer configuration with XTime_StartTTCTimer.
;                     Now the timer instance as specified by the user will be
;		      started.
; 6.6  mus   02/23/17 Disable the debug logic in non-JTAG boot mode(when
;		      processor is in lockstep configuration), based
;		      on the mld parameter "lockstep_mode_debug".
* 6.8  mus   09/20/18 Clear VINITHI field in RPU_0_CFG/RPU_1_CFG
*		      registers to initialize CortexR5 core with LOVEC
*		      on reset. It fixes CR#1010656.
* 7.0  mus   03/19/19  Disable FPU only in case of softp, otherwise enable it by
*		      default. CR#1021638
;
;  </pre>
;
;  @note
;
; None.
;
; *****************************************************************************/

        MODULE  ?boot
        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION SVC_STACK:DATA:NOROOT(3)
        SECTION ABT_STACK:DATA:NOROOT(3)
        SECTION UND_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)

#include "xparameters.h"

#define UART_BAUDRATE	115200
	PUBLIC _prestart
	PUBLIC __iar_program_start
	IMPORT _vector_table
        IMPORT Init_MPU
#ifdef SLEEP_TIMER_BASEADDR
	IMPORT XTime_StartTTCTimer
#endif
	IMPORT __cmain
vector_base     EQU     _vector_table
RPU_GLBL_CNTL   EQU     0xFF9A0000
RPU_ERR_INJ     EQU	0xFF9A0020
RPU_0_CFG       EQU     0xFF9A0100
RPU_1_CFG       EQU     0xFF9A0200
RST_LPD_DBG     EQU	0xFF5E0240
BOOT_MODE_USER  EQU	0xFF5E0200
fault_log_enable EQU     0x101

	SECTION .boot:CODE:NOROOT(2)


/* this initializes the various processor modes */

_prestart
__iar_program_start

OKToRun

        REQUIRE _vector_table
/* Initialize processor registers to 0 */
	mov	r0,#0
	mov	r1,#0
	mov	r2,#0
	mov	r3,#0
	mov	r4,#0
	mov	r5,#0
	mov	r6,#0
	mov	r7,#0
	mov	r8,#0
	mov	r9,#0
	mov	r10,#0
	mov	r11,#0
	mov	r12,#0

/* Initialize stack pointer and banked registers for various mode */
	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the irq stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x12			; IRQ mode
	msr	cpsr, r2
	ldr	r13,=SFE(IRQ_STACK)			; IRQ stack pointer
	mov 	r14,#0

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the supervisor stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x13			; supervisor mode
	msr	cpsr, r2
	ldr	r13,=SFE(SVC_STACK)			; Supervisor stack pointer
	mov 	r14,#0

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the Abort  stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x17			; Abort mode
	msr	cpsr, r2
	ldr	r13,=SFE(ABT_STACK)		; Abort stack pointer
	mov 	r14,#0

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the FIQ stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x11			; FIQ mode
	msr	cpsr, r2
	mov 	r8, #0
	mov 	r9, #0
	mov 	r10, #0
	mov 	r11, #0
	mov 	r12, #0
	ldr	r13,=SFE(FIQ_STACK)		; FIQ stack pointer
	mov 	r14,#0

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the Undefine stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x1b			; Undefine mode
	msr	cpsr, r2
	ldr	r13,=SFE(UND_STACK)		; Undefine stack pointer
	mov 	r14,#0

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the system stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x1F			; SYS mode
	msr	cpsr, r2
	ldr	r13,=SFE(CSTACK)		; SYS stack pointer
	mov 	r14,#0

 ;
 ; Enable access to VFP by enabling access to Coprocessors 10 and 11.
 ; Enables Full Access i.e. in both privileged and non privileged modes
 ;
	mrc     p15, 0, r0, c1, c0, 2      	; Read Coprocessor Access Control Register (CPACR)
        orr     r0, r0, #(0xF << 20)       	; Enable access to CP 10 & 11
        mcr     p15, 0, r0, c1, c0, 2      	; Write Coprocessor Access Control Register (CPACR)
        isb

; enable fpu access
	vmrs	r3, FPEXC
	orr	r1, r3, #(1<<30)
	vmsr	FPEXC, r1

 ; clear the floating point register
	mov	r1,#0
	vmov	d0,r1,r1
	vmov	d1,r1,r1
	vmov	d2,r1,r1
	vmov	d3,r1,r1
	vmov	d4,r1,r1
	vmov	d5,r1,r1
	vmov	d6,r1,r1
	vmov	d7,r1,r1
	vmov	d8,r1,r1
	vmov	d9,r1,r1
	vmov	d10,r1,r1
	vmov	d11,r1,r1
	vmov	d12,r1,r1
	vmov	d13,r1,r1
	vmov	d14,r1,r1
	vmov	d15,r1,r1

#ifdef __SOFTFP__
 ; Disable FPU by restoring previous value for FPU access
	vmsr	FPEXC,r3
#endif

 ; Disable MPU and caches
        mrc     p15, 0, r0, c1, c0, 0       	 ; Read CP15 Control Register
        bic     r0, r0, #0x05               	 ; Disable MPU (M bit) and data cache (C bit)
        bic     r0, r0, #0x1000             	 ; Disable instruction cache (I bit)
        dsb                                 	 ; Ensure all previous loads/stores have completed
        mcr     p15, 0, r0, c1, c0, 0       	 ; Write CP15 Control Register
        isb                                 	 ; Ensure subsequent insts execute wrt new MPU settings

 ; Disable Branch prediction, TCM ECC checks
        mrc     p15, 0, r0, c1, c0, 1       	 ; Read ACTLR
        orr     r0, r0, #(0x1 << 17)        	 ; Enable RSDIS bit 17 to disable the return stack
        orr     r0, r0, #(0x1 << 16)        	 ; Clear BP bit 15 and set BP bit 16
        bic     r0, r0, #(0x1 << 15)        	 ; Branch always not taken and history table updates disabled
        bic     r0, r0, #(0x1 << 27)		 ; Disable B1TCM ECC check
        bic     r0, r0, #(0x1 << 26)		 ; Disable B0TCM ECC check
        bic     r0, r0, #(0x1 << 25)		 ; Disable ATCM ECC check
	orr	r0, r0, #(0x1 << 5)		 ; Enable ECC with no forced write through with [5:3]=b'101
	bic 	r0, r0, #(0x1 << 4)
	orr	r0, r0, #(0x1 << 3)
        mcr     p15, 0, r0, c1, c0, 1       	 ; Write ACTLR*/
	dsb				    	 ; Complete all outstanding explicit memory operations*/

 ; Invalidate caches
	mov	r0,#0				 ; r0 = 0
	dsb
	mcr	p15, 0, r0, c7, c5, 0		 ; invalidate icache
	mcr 	p15, 0, r0, c15, c5, 0      	 ; Invalidate entire data cache
	isb

#if LOCKSTEP_MODE_DEBUG == 0
 ; enable fault log for lock step
	ldr	r0,=RPU_GLBL_CNTL
	ldr	r1, [r0]
	ands	r1, r1, #0x8
 ; branch to initialization if split mode
	bne 	init
 ; check for boot mode if in lock step, branch to init if JTAG boot mode
	ldr	r0,=BOOT_MODE_USER
	ldr 	r1, [r0]
	ands	r1, r1, #0xF
	beq 	init
 ; reset the debug logic
	ldr	r0,=RST_LPD_DBG
	ldr	r1, [r0]
	orr	r1, r1, #(0x1 << 1)
	orr	r1, r1, #(0x1 << 4)
	orr	r1, r1, #(0x1 << 5)
	str	r1, [r0]
 ; enable fault log
	ldr	r0,=RPU_ERR_INJ
	ldr	r1,=fault_log_enable
	ldr	r2, [r0]
	orr	r2, r2, r1
	str	r2, [r0]
	nop
	nop
 #endif

init
	bl 	Init_MPU		 ; Initialize MPU

 ; Enable Branch prediction
	mrc     p15, 0, r0, c1, c0, 1        ; Read ACTLR
        bic     r0, r0, #(0x1 << 17)         ; Clear RSDIS bit 17 to enable return stack
        bic     r0, r0, #(0x1 << 16)         ; Clear BP bit 15 and BP bit 16:
        bic     r0, r0, #(0x1 << 15)         ; Normal operation, BP is taken from the global history table.
        orr	r0, r0, #(0x1 << 14)	     ; Disable DBWR for errata 780125
	mcr     p15, 0, r0, c1, c0, 1        ; Write ACTLR

 ; Enable icahce and dcache
	mrc 	p15,0,r1,c1,c0,0
	ldr	r0, =0x1005
	orr 	r1,r1,r0
	dsb
	mcr	p15,0,r1,c1,c0,0		 ; Enable cache
	isb					 ; isb	flush prefetch buffer



 ; Set vector table in TCM/LOVEC
#ifndef VEC_TABLE_IN_OCM
	mrc	p15, 0, r0, c1, c0, 0
	mvn	r1, #0x2000
	and	r0, r0, r1
	mcr	p15, 0, r0, c1, c0, 0

 ; Clear VINITHI to enable LOVEC on reset
#if XPAR_CPU_ID == 0
	ldr	r0, =RPU_0_CFG
#else
	ldr	r0, =RPU_1_CFG
#endif
	ldr	r1, [r0]
	bic	r1, r1, #(0x1 << 2)
	str	r1, [r0]
#endif

 ; enable asynchronous abort exception
	mrs	r0, cpsr
	bic	r0, r0, #0x100
	msr	cpsr_xsf, r0

        ; Clear cp15 regs with unknown reset values
	mov	r0, #0x0
	mcr	p15, 0, r0, c5, c0, 0	; DFSR
	mcr	p15, 0, r0, c5, c0, 1	; IFSR
	mcr	p15, 0, r0, c6, c0, 0	; DFAR
	mcr	p15, 0, r0, c6, c0, 2	; IFAR
	mcr	p15, 0, r0, c9, c13, 2	; PMXEVCNTR
	mcr	p15, 0, r0, c13, c0, 2	; TPIDRURW
	mcr	p15, 0, r0, c13, c0, 3	; TPIDRURO


        ; Reset and start Cycle Counter
	mov	r2, #0x80000000		; clear overflow
	mcr	p15, 0, r2, c9, c12, 3
	mov	r2, #0xd		; D, C, E
	mcr	p15, 0, r2, c9, c12, 0
	mov	r2, #0x80000000		; enable cycle counter
	mcr	p15, 0, r2, c9, c12, 1

	; configure the timer if TTC is present
        #ifdef SLEEP_TIMER_BASEADDR
	    bl XTime_StartTTCTimer
        #endif

        ; make sure argc and argv are valid
	mov	r0, #0
	mov	r1, #0

        b 	__cmain                        ; jump to C startup code


Ldone	b	Ldone				 ; Paranoia: we should never get here


      END
