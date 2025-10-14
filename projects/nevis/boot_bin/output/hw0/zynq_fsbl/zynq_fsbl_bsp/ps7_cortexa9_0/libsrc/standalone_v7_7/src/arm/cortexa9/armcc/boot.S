;******************************************************************************
; Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
; SPDX-License-Identifier: MIT
;*****************************************************************************
;****************************************************************************
;**
; @file boot.S
;
; This file contains the initial startup code for the Cortex A9 processor
;
; <pre>
; MODIFICATION HISTORY:
;
; Ver	Who	  Date	  Changes
; ----- ------- -------- ---------------------------------------------------
; 1.00a ecm/sdm 10/20/09 Initial version
; 3.04a sdm	01/02/12 Updated to clear cp15 regs with unknown reset values
; 3.06a sgd     05/15/12 Updated L2CC Auxiliary and Tag RAM Latency control
;			 register settings.
; 3.06a asa 	06/17/12 Modified the TTBR settings and L2 Cache auxiliary
;		         register settings.
; 3.07a sgd 	07/05/12 Updated with reset and start Global Timer
; 3.07a sgd 	10/19/12 SMC NOR and SRAM initialization with build option
; 4.2   pkp	06/19/14 Enabled asynchronous abort exception
; 4.2	pkp  	08/04/14 Removed PEEP board related code which contained
;			 initialization of uart smc nor and sram
; 5.0	pkp	16/12/14 Modified initialization code to enable scu after
;			 MMU is enabled and removed incorrect initialization
;			 of TLB lockdown register to fix CR#830580
; 5.1   pkp	05/13/15 Changed the initialization order so to first invalidate
;			 caches and TLB, enable MMU and caches, then enable SMP
;			 bit in ACTLR. L2Cache invalidation and enabling of L2Cache
;			 is done later.
; 5.4  asa      12/06/15 Added code to initialize SPSR for all relevant modes.
; 6.0  mus      04/08/16 Added code to detect zynq-7000 base silicon configuration and
;                        attempt to enable dual core behavior on single cpu zynq-7000s devices
;                        is prevented from corrupting system behavior.
; 6.0  mus      24/08/16 Check CPU core before putting cpu1 to reset for single core
;                        zynq-7000s devices
; 6.6   srm	  10/25/17  Added timer configuration using XTime_StartTTCTimer API.
;		      	        Now the TTC instance as specified by the user will be
;					    started.
; 7.7   asa     01/06/22 Removed Cortex-A9 errata handling for errata
; 			 742230 and 743622. These do not apply to
; 			 Cortex-A9 revision r3p0 being used in Zynq
; 			 platforms.
; </pre>
;
; @note
;
; None.
;
;****************************************************************************



#include "xparameters.h"
#include "xil_errata.h"

#define UART_BAUDRATE	115200

	EXPORT _prestart
	EXPORT _boot

	IMPORT |Image$$ARM_LIB_STACK$$ZI$$Limit|
	IMPORT |Image$$IRQ_STACK$$ZI$$Limit|
	IMPORT |Image$$SPV_STACK$$ZI$$Limit|
	IMPORT |Image$$ABORT_STACK$$ZI$$Limit|
	IMPORT MMUTable
	IMPORT _vector_table
	IMPORT __main
	IMPORT Xil_ExceptionInit
	IMPORT XTime_SetTime
#if defined SLEEP_TIMER_BASEADDR
	IMPORT XTime_StartTTCTimer
#endif

PSS_L2CC_BASE_ADDR	EQU	0xF8F02000
PSS_SLCR_BASE_ADDR 	EQU	0xF8000000

L2CCWay		EQU	(PSS_L2CC_BASE_ADDR + 0x077C)	;(PSS_L2CC_BASE_ADDR + PSS_L2CC_CACHE_INVLD_WAY_OFFSET)
L2CCSync	EQU	(PSS_L2CC_BASE_ADDR + 0x0730)	;(PSS_L2CC_BASE_ADDR + PSS_L2CC_CACHE_SYNC_OFFSET)
L2CCCrtl	EQU	(PSS_L2CC_BASE_ADDR + 0x0100)	;(PSS_L2CC_BASE_ADDR + PSS_L2CC_CNTRL_OFFSET)
L2CCAuxCrtl	EQU	(PSS_L2CC_BASE_ADDR + 0x0104)	;(PSS_L2CC_BASE_ADDR + XPSS_L2CC_AUX_CNTRL_OFFSET)
L2CCTAGLatReg	EQU	(PSS_L2CC_BASE_ADDR + 0x0108)	;(PSS_L2CC_BASE_ADDR + XPSS_L2CC_TAG_RAM_CNTRL_OFFSET)
L2CCDataLatReg	EQU	(PSS_L2CC_BASE_ADDR + 0x010C)	;(PSS_L2CC_BASE_ADDR + XPSS_L2CC_DATA_RAM_CNTRL_OFFSET)
L2CCIntClear	EQU	(PSS_L2CC_BASE_ADDR + 0x0220)	;(PSS_L2CC_BASE_ADDR + XPSS_L2CC_IAR_OFFSET)
L2CCIntRaw	EQU	(PSS_L2CC_BASE_ADDR + 0x021C)	;(PSS_L2CC_BASE_ADDR + XPSS_L2CC_ISR_OFFSET)

SLCRlockReg	EQU    (PSS_SLCR_BASE_ADDR + 0x04)	/*(PSS_SLCR_BASE_ADDR + XPSS_SLCR_LOCK_OFFSET)*/
SLCRUnlockReg   EQU    (PSS_SLCR_BASE_ADDR + 0x08)	/*(PSS_SLCR_BASE_ADDR + XPSS_SLCR_UNLOCK_OFFSET)*/
SLCRL2cRamReg   EQU    (PSS_SLCR_BASE_ADDR + 0xA1C) /*(PSS_SLCR_BASE_ADDR + XPSS_SLCR_L2C_RAM_OFFSET)*/
SLCRCPURSTReg   EQU    (0xF8000000 + 0x244)             ;(XPS_SYS_CTRL_BASEADDR + A9_CPU_RST_CTRL_OFFSET)
EFUSEStaus      EQU    (0xF800D000 + 0x10)              ;(XPS_EFUSE_BASEADDR + EFUSE_STATUS_OFFSET)

SLCRlockKey	EQU        	0x767B			/* SLCR lock key */
SLCRUnlockKey	EQU        	0xDF0D			/* SLCR unlock key */
SLCRL2cRamConfig EQU      	0x00020202      /* SLCR L2C ram configuration */

CRValMmuCac		EQU	2_01000000000101	; Enable IDC, and MMU
CRValHiVectorAddr	EQU	2_10000000000000	; Set the Vector address to high, 0xFFFF0000

L2CCAuxControl	EQU	0x72360000	; Enable all prefetching, Way Size (16 KB) and High Priority for SO and Dev Reads Enable
L2CCControl	EQU	0x01		; Enable L2CC
L2CCTAGLatency	EQU	0x0111		; 7 Cycles of latency for TAG RAM
L2CCDataLatency	EQU	0x0121		; 7 Cycles of latency for DATA RAM

FPEXC_EN	EQU	0x40000000	; FPU enable bit, (1 << 30)

	AREA |.boot|, CODE
	PRESERVE8


; this initializes the various processor modes

_prestart
_boot

#if XPAR_CPU_ID==0
; only allow cp0 through
	mrc	p15,0,r1,c0,c0,5
	and	r1, r1, #0xf
	cmp	r1, #0
	beq	CheckEFUSE
EndlessLoop0
	wfe
	b	EndlessLoop0

CheckEFUSE
        ldr r0,=EFUSEStaus
        ldr r1,[r0]                             ; Read eFuse setting
        ands r1,r1,#0x80                        ; Check whether device is having single core
	beq OKToRun
 ; Single core device, reset CPU1
	ldr	r0,=SLCRUnlockReg		; Load SLCR base address base + unlock register
	ldr	r1,=SLCRUnlockKey	   	; set unlock key
	str	r1, [r0]		   	; Unlock SLCR

	ldr r0,=SLCRCPURSTReg
	ldr r1,[r0]                             ; Read CPU Software Reset Control register
	orr r1,r1,#0x22
        str r1,[r0]                             ; Reset CPU1

	ldr	r0,=SLCRlockReg         	; Load SLCR base address base + lock register
	ldr	r1,=SLCRlockKey	        	; set lock key
	str	r1, [r0]	        	; lock SLCR

#elif XPAR_CPU_ID==1
; only allow cpu1 through
	mrc	p15,0,r1,c0,c0,5
	and	r1, r1, #0xf
	cmp	r1, #1
	beq	CheckEFUSE1
        b	EndlessLoop1

CheckEFUSE1
        ldr r0,=EFUSEStaus
        ldr r1,[r0]                             ; Read eFuse setting
        ands r1,r1,#0x80                        ; Check whether device is having single core
	beq OKToRun
EndlessLoop1
	wfe
	b	EndlessLoop1
#endif

OKToRun
	mrc     p15, 0, r0, c0, c0, 0		/* Get the revision */
	and     r5, r0, #0x00f00000
	and     r6, r0, #0x0000000f
	orr     r6, r6, r5, lsr #20-4

	/* set VBAR to the _vector_table address in scatter file */
	ldr	r0, =_vector_table
	mcr	p15, 0, r0, c12, c0, 0

	;invalidate scu
	ldr	r7, =0xf8f0000c
	ldr	r6, =0xffff
	str	r6, [r7]

	;Invalidate caches and TLBs
	mov	r0,#0				; r0 = 0
	mcr	p15, 0, r0, c8, c7, 0		; invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0		; invalidate icache
	mcr	p15, 0, r0, c7, c5, 6		; Invalidate branch predictor array
	bl	invalidate_dcache		; invalidate dcache

	; Disable MMU, if enabled
	mrc	p15, 0, r0, c1, c0, 0		; read CP15 register 1
	bic	r0, r0, #0x1			; clear bit 0
	mcr	p15, 0, r0, c1, c0, 0		; write value back

#ifdef SHAREABLE_DDR
	; Mark the entire DDR memory as shareable
	ldr	r3, =0x3ff			; 1024 entries to cover 1G DDR
	ldr	r0, =TblBase			; MMU Table address in memory
	ldr	r2, =0x15de6			; S=1, TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1
shareable_loop:
	str	r2, [r0]			; write the entry to MMU table
	add	r0, r0, #0x4			; next entry in the table
	add	r2, r2, #0x100000		; next section
	subs	r3, r3, #1
	bge	shareable_loop			; loop till 1G is covered
#endif

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the irq stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x12			; IRQ mode
	msr	apsr, r2			; was cpsr, apsr is considered synonym
	ldr	r13,=|Image$$IRQ_STACK$$ZI$$Limit|	; IRQ stack pointer
	bic r2, r2, #(0x1 << 9)    		; Set EE bit to little-endian
	msr spsr_fsxc,r2

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the supervisor stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x13			; supervisor mode
	msr	apsr, r2			; was cpsr, apsr is considered synonym
	ldr	r13,=|Image$$SPV_STACK$$ZI$$Limit|	; Supervisor stack pointer
	bic r2, r2, #(0x1 << 9)    		; Set EE bit to little-endian
	msr spsr_fsxc,r2

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the Abort  stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x17			; Abort mode
	msr	apsr, r2			; was cpsr, apsr is considered synonym
	ldr	r13,=|Image$$ABORT_STACK$$ZI$$Limit|	; Abort stack pointer
	bic r2, r2, #(0x1 << 9)    		; Set EE bit to little-endian
	msr spsr_fsxc,r2

	mrs	r0, cpsr			; get the current PSR
	mvn	r1, #0x1f			; set up the system stack pointer
	and	r2, r1, r0
	orr	r2, r2, #0x1f			; SYS mode
	msr	apsr, r2			 ; was cpsr, apsr is considered synonym
	ldr	r13,=|Image$$ARM_LIB_STACK$$ZI$$Limit|	; SYS stack pointer

	;set scu enable bit in scu
	ldr	r7, =0xf8f00000
	ldr	r0, [r7]
	orr	r0, r0, #0x1
	str	r0, [r7]

	; enable MMU and cache

	ldr	r0,=MMUTable			; Load MMU translation table base
	orr	r0, r0, #0x5B			; Outer-cacheable, WB
	mcr	p15, 0, r0, c2, c0, 0		; TTB0

	mvn	r0,#0
	mcr	p15,0,r0,c3,c0,0

	; Enable mmu, icahce and dcache
	ldr	r0,=CRValMmuCac

	mcr	p15,0,r0,c1,c0,0		; Enable cache and MMU
	dsb					; dsb allow the MMU to start up
	isb					; isb flush prefetch buffer


	; Write to ACTLR
	mrc	p15, 0,r0, c1, c0, 1		; Read ACTLR
	orr	r0, r0, #(0x01 << 6)		; SMP bit
	orr	r0, r0, #(0x01 )		; Cache/TLB maintenance broadcast
	mcr	p15, 0,r0, c1, c0, 1		; Write ACTLR

; Invalidate L2 Cache and initialize L2 Cache
; For AMP, assume running on CPU1. Don't initialize L2 Cache (up to Linux)
#if USE_AMP!=1
	ldr	r0,=L2CCCrtl			; Load L2CC base address base + control register
	mov	r1, #0				; force the disable bit
	str	r1, [r0]			; disable the L2 Caches

	ldr	r0,=L2CCAuxCrtl			; Load L2CC base address base + Aux control register
	ldr	r1,[r0]				; read the register
	ldr	r2,=L2CCAuxControl		; set the default bits
	orr	r1,r1,r2
	str	r1, [r0]			; store the Aux Control Register

	ldr	r0,=L2CCTAGLatReg		; Load L2CC base address base + TAG Latency address
	ldr	r1,=L2CCTAGLatency		; set the latencies for the TAG
	str	r1, [r0]			; store the TAG Latency register Register

	ldr	r0,=L2CCDataLatReg		; Load L2CC base address base + Data Latency address
	ldr	r1,=L2CCDataLatency		; set the latencies for the Data
	str	r1, [r0]			; store the Data Latency register Register

	ldr	r0,=L2CCWay			; Load L2CC base address base + way register
	ldr	r2, =0xFFFF
	str	r2, [r0]			; force invalidate

	ldr	r0,=L2CCSync			; need to poll 0x730, PSS_L2CC_CACHE_SYNC_OFFSET
						; Load L2CC base address base + sync register
	; poll for completion
Sync
	ldr	r1, [r0]
	cmp	r1, #0
	bne	Sync

	ldr	r0,=L2CCIntRaw			; clear pending interrupts
	ldr	r1,[r0]
	ldr	r0,=L2CCIntClear
	str	r1,[r0]

	ldr	r0,=SLCRUnlockReg		;Load SLCR base address base + unlock register
	ldr	r1,=SLCRUnlockKey	   	;set unlock key
	str	r1, [r0]		   	;Unlock SLCR

	ldr	r0,=SLCRL2cRamReg		;Load SLCR base address base + l2c Ram Control register
	ldr	r1,=SLCRL2cRamConfig        	;set the configuration value
	str	r1, [r0]	        	;store the L2c Ram Control Register

	ldr	r0,=SLCRlockReg         	;Load SLCR base address base + lock register
	ldr	r1,=SLCRlockKey	        	;set lock key
	str	r1, [r0]	        	;lock SLCR

	ldr	r0,=L2CCCrtl			; Load L2CC base address base + control register
	ldr	r1,[r0]				; read the register
	mov	r2, #L2CCControl		; set the enable bit
	orr	r1,r1,r2
	str	r1, [r0]			; enable the L2 Caches
#endif

	mov	r0, r0
	mrc	p15, 0, r1, c1, c0, 2		; read cp access control register (CACR) into r1
	orr	r1, r1, #(0xf << 20)		; enable full access for p10 & p11
	mcr	p15, 0, r1, c1, c0, 2		; write back into CACR

	; enable vfp
	fmrx  r1, FPEXC				; read the exception register
	orr	r1,r1, #FPEXC_EN		; set VFP enable bit, leave the others in orig state
	fmxr  FPEXC, r1				; write back the exception register

	mrc	p15, 0, r0, c1, c0, 0		; flow prediction enable
	orr	r0, r0, #(0x01 << 11)		; #0x8000
	mcr	p15,0,r0,c1,c0,0

	mrc	p15, 0, r0, c1, c0, 1		; read Auxiliary Control Register
	orr	r0, r0, #(0x1 << 2)		; enable Dside prefetch
	orr	r0, r0, #(0x1 << 1)		; enable L2 prefetch
	mcr	p15, 0, r0, c1, c0, 1		; write Auxiliary Control Register

	mrs	r0, cpsr			/* get the current PSR */
	bic	r0, r0, #0x100			/* enable asynchronous abort exception */
	msr	cpsr_xsf, r0

; Clear cp15 regs with unknown reset values
	mov	r0, #0x0
	mcr	p15, 0, r0, c5, c0, 0		; DFSR
	mcr	p15, 0, r0, c5, c0, 1		; IFSR
	mcr	p15, 0, r0, c6, c0, 0		; DFAR
	mcr	p15, 0, r0, c6, c0, 2		; IFAR
	mcr	p15, 0, r0, c9, c13, 2		; PMXEVCNTR
	mcr	p15, 0, r0, c13, c0, 2		; TPIDRURW
	mcr	p15, 0, r0, c13, c0, 3		; TPIDRURO

; Reset and start Cycle Counter
	mov	r2, #0x80000000			; clear overflow
	mcr	p15, 0, r2, c9, c12, 3
	mov	r2, #0xd			; D, C, E
	mcr	p15, 0, r2, c9, c12, 0
	mov	r2, #0x80000000			; enable cycle counter
	mcr	p15, 0, r2, c9, c12, 1

; Reset and start Global Timer
    mov	r0, #0x0
    mov	r1, #0x0
    bl XTime_SetTime

; Reset and start Triple Timer counter
	#if defined SLEEP_TIMER_BASEADDR
	bl XTime_StartTTCTimer
	#endif

#ifdef PROFILING			/* defined in Makefile */
	/* Setup profiling stuff */
	bl	_profile_init
#endif /* PROFILING */

; make sure argc and argv are valid
	mov r0, #0
	mov r1, #0
	b  __main				; jump to C startup code
	and	r0, r0, r0			; no op

Ldone  b  Ldone					; Paranoia: we should never get here


; *************************************************************************
; *
; * invalidate_dcache - invalidate the entire d-cache by set/way
; *
; * Note: for Cortex-A9, there is no cp instruction for invalidating
; * the whole D-cache. Need to invalidate each line.
; *
; *************************************************************************

invalidate_dcache
	mrc	p15, 1, r0, c0, c0, 1		; read CLIDR
	ands	r3, r0, #0x7000000
	mov	r3, r3, lsr #23			; cache level value (naturally aligned)
	beq	finished
	mov	r10, #0				; start with level 0
loop1
	add	r2, r10, r10, lsr #1		; work out 3xcachelevel
	mov	r1, r0, lsr r2			; bottom 3 bits are the Cache type for this level
	and	r1, r1, #7			; get those 3 bits alone
	cmp	r1, #2
	blt	skip				; no cache or only instruction cache at this level
	mcr	p15, 2, r10, c0, c0, 0		; write the Cache Size selection register
	isb					; isb to sync the change to the CacheSizeID reg
	mrc	p15, 1, r1, c0, c0, 0		; reads current Cache Size ID register
	and	r2, r1, #7			; extract the line length field
	add	r2, r2, #4			; add 4 for the line length offset (log2 16 bytes)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		; r4 is the max number on the way size (right aligned)
	clz	r5, r4				; r5 is the bit position of the way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		; r7 is the max number of the index size (right aligned)
loop2
	mov	r9, r4				; r9 working copy of the max way size (right aligned)
loop3
	orr	r11, r10, r9, lsl r5		; factor in the way number and cache number into r11
	orr	r11, r11, r7, lsl r2		; factor in the index number
	mcr	p15, 0, r11, c7, c6, 2		; invalidate by set/way
	subs	r9, r9, #1			; decrement the way number
	bge	loop3
	subs	r7, r7, #1			; decrement the index
	bge	loop2
skip
	add	r10, r10, #2			; increment the cache number
	cmp	r3, r10
	bgt	loop1

finished
	mov	r10, #0				; switch back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		; select current cache level in cssr
	isb

	bx lr

	END
