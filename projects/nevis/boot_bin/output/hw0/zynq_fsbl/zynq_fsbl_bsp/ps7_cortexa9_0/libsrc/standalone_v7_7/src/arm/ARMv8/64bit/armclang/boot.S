;/******************************************************************************
;* Copyright (c) 2019 - 2021 Xilinx, Inc.  All rights reserved.
;* SPDX-License-Identifier: MIT
;******************************************************************************/
;/*****************************************************************************/
;/**
;* @file boot.S
;*
;* @addtogroup a53_64_boot_code Cortex A53 64bit Processor Boot Code
;* @{
;* <h2> boot.S </h2>
;*
;* The boot code performs minimum configuration which is required for an
;* application. Cortex-A53 starts by checking current exception level. If the
;* current exception level is EL3 and BSP is built for EL3, it will do
;* initialization required for application execution at EL3. Below is a
;* sequence illustrating what all configuration is performed before control
;* reaches to main function for EL3 execution.
;*
;* 1. Program vector table base for exception handling
;* 2. Set reset vector table base address
;* 3. Program stack pointer for EL3
;* 4. Routing of interrupts to EL3
;* 5. Enable ECC protection
;* 6. Program generic counter frequency
;* 7. Invalidate instruction cache, data cache and TLBs
;* 8. Configure MMU registers and program base address of translation table
;* 9. Transfer control to _start which clears BSS sections and runs global
;*    constructor before jumping to main application
;*
;* If current exception level is EL1 and BSP is also built for EL1_NONSECURE
;* it will perform initialization required for application execution at EL1
;* non-secure. For all other combination, the execution will go into infinite
;* loop. Below is a sequence illustrating what all configuration is performed
;* before control reaches to main function for EL1 execution.
;*
;* 1. Program vector table base for exception handling
;* 2. Program stack pointer for EL1
;* 3. Invalidate instruction cache, data cache and TLBs
;* 4. Configure MMU registers and program base address of translation table
;* 5. Transfer control to _start which clears BSS sections and runs global
;*    constructor before jumping to main application
;*
;* <pre>
;* MODIFICATION HISTORY:
;*
;* Ver   Who     Date     Changes
;* ----- ------- -------- ---------------------------------------------------
;* 7.0   mus     02/26/19 First release
;* 7.2   mus     01/08/19 Added support for versal
;*       sd      02/23/20 Clock Init is called
;*       sd      03/21/20 Added XCLOCKING flag
;* </pre>
;******************************************************************************/

#include "xparameters.h"
#include "bspconfig.h"
#include "xil_errata.h"

	EXPORT _prestart
	EXPORT _boot

	IMPORT MMUTableL0
	IMPORT |Image$$ARM_LIB_STACK$$ZI$$Base|
	IMPORT _vector_table
	IMPORT __main
#ifdef XCLOCKING
	IMPORT Xil_ClockInit
#endif

#ifndef FREERTOS_BSP
	IMPORT FPUStatus
#endif

rvbar_base  EQU 0xFD5C0040
MODE_EL1    EQU 0x5
DAIF_BIT    EQU 0x1C0
TT_S1_FAULT EQU 0x0
TT_S1_TABLE EQU 0x3

	AREA |.boot|, CODE

; This initializes the various processor modes

_prestart
_boot
   mov	x0, #0
   mov	x1, #0
   mov	x2, #0
   mov	x3, #0
   mov	x4, #0
   mov	x5, #0
   mov	x6, #0
   mov	x7, #0
   mov	x8, #0
   mov	x9, #0
   mov	x10, #0
   mov	x11, #0
   mov	x12, #0
   mov	x13, #0
   mov	x14, #0
   mov	x15, #0
   mov	x16, #0
   mov	x17, #0
   mov	x18, #0
   mov	x19, #0
   mov	x20, #0
   mov	x21, #0
   mov	x22, #0
   mov	x23, #0
   mov	x24, #0
   mov	x25, #0
   mov	x26, #0
   mov	x27, #0
   mov	x28, #0
   mov	x29, #0
   mov	x30, #0

OKToRun
   mrs	x0, currentEL
   cmp	x0, #0xC
   beq	InitEL3
   cmp	x0, #0x4
   beq	InitEL1
   b	error					; Go to error if current exception level is neither EL3 nor EL1


InitEL3
#if (EL3 == 1)
   ldr	x1, =_vector_table			; Set vector table base address
   msr	VBAR_EL3, x1
   mrs	x0, MPIDR_EL1				; Get the CPU ID
   and	x0, x0, #0xFF
   mov	w0, w0
   ldr	w2, =rvbar_base				; Calculate the rvbar base address for particular CPU core
   mov	w3, #0x8
   mul	w0, w0, w3
   add	w2, w2, w0
   str	x1, [x2]				; Store vector base address to rvbar
   ldr	x2, =|Image$$ARM_LIB_STACK$$ZI$$Base|	; Define stack pointer for current exception level
   mov	sp, x2

   mov	x0, #0					; Enable Trapping of SIMD/FPU register for standalone BSP
#ifndef FREERTOS_BSP
   orr	x0, x0, #(0x1 << 10)
#endif
   msr	CPTR_EL3, x0
   isb

;
; Clear FPUStatus variable to make sure that it contains current
; status of FPU i.e. disabled. In case of a warm restart execution
; when bss sections are not cleared, it may contain previously updated
; value which does not hold true now
;
#ifndef FREERTOS_BSP
   ldr	x0, =FPUStatus
   str	xzr, [x0]
#endif

; Configure SCR_EL3
   mov	w1, #0					; Initial value of register is unknown
   orr	w1, w1, #(1 << 11)			; Set ST bit (Secure EL1 can access CNTPS_TVAL_EL1, CNTPS_CTL_EL1 & CNTPS_CVAL_EL1)
   orr	w1, w1, #(1 << 10)			; Set RW bit (EL1 is AArch64, as this is the Secure world)
   orr	w1, w1, #(1 << 3)			; Set EA bit (SError routed to EL3)
   orr	w1, w1, #(1 << 2)			; Set FIQ bit (FIQs routed to EL3)
   orr	w1, w1, #(1 << 1)			; Set IRQ bit (IRQs routed to EL3)
   msr	SCR_EL3, x1

; Configure cpu auxiliary control register EL1
   ldr	x0, =0x80CA000				; L1 Data prefetch control - 5, Enable device split throttle, 2 independent data prefetch streams
#if (CONFIG_ARM_ERRATA_855873)
;
; Set ENDCCASCI bit in CPUACTLR_EL1 register, to execute data
; cache clean operations as data cache clean and invalidate
;
   orr	x0, x0, #(1 << 44)			; Set ENDCCASCI bit
#endif
   msr	S3_1_C15_C2_0, x0			; CPUACTLR_EL1

; Program the counter frequency
#if defined (versal)
   ldr	x0, =XPAR_CPU_CORTEXA72_0_TIMESTAMP_CLK_FREQ
#else
   ldr	x0, =XPAR_CPU_CORTEXA53_0_TIMESTAMP_CLK_FREQ
   msr	CNTFRQ_EL0, x0
#endif

; Enable hardware coherency between cores
   mrs	x0, S3_1_c15_c2_1			; Read EL1 CPU Extended Control Register
   orr	x0, x0, #(1 << 6)			; Set the SMPEN bit
   msr	S3_1_c15_c2_1, x0			; Write EL1 CPU Extended Control Register
   isb

   tlbi	ALLE3
   ic	IALLU					; Invalidate I cache to PoU
   bl	invalidate_dcaches
   dsb	sy
   isb

   ldr	x1, =MMUTableL0				; Get address of level 0 for TTBR0_EL3
   msr	TTBR0_EL3, x1				; Set TTBR0_EL3

;
; Set up memory attributes
; This equates to:
; 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
; 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
; 2 = b00000000 = Device-nGnRnE
; 3 = b00000100 = Device-nGnRE
; 4 = b10111011 = Normal, Inner/Outer WT/WA/RA
;
   ldr	x1, =0x000000BB0400FF44
   msr	MAIR_EL3, x1
#if defined (versal)
; Set up TCR_EL3
; Physical Address Size PS =  100 -> 44bits 16 TB
; Granual Size TG0 = 00 -> 4KB
; size offset of the memory region T0SZ = 20 -> (region size 2^(64-20) = 2^44)
    ldr     x1,=0x80843514
#else
;
; Set up TCR_EL3
; Physical Address Size PS =  010 -> 40bits 1TB
; Granule Size TG0 = 00 -> 4KB
; Size offset of the memory region T0SZ = 24 -> (region size 2^(64-24) = 2^40)
;
   ldr	x1, =0x80823518
#endif
   msr	TCR_EL3, x1
   isb


; Enable SError Exception for asynchronous abort
   mrs	x1, DAIF
   mov	x2, #(0x1<<8)
   bic	x1, x1, x2
   msr	DAIF, x1


; Configure SCTLR_EL3
   mov	x1, #0					; Most of the SCTLR_EL3 bits are unknown at reset
   orr	x1, x1, #(1 << 12)			; Enable I cache
   orr	x1, x1, #(1 << 3)			; Enable SP alignment check
   orr	x1, x1, #(1 << 2)			; Enable caches
   orr	x1, x1, #(1 << 0)			; Enable MMU
   msr	SCTLR_EL3, x1
   dsb	sy
   isb

#ifdef XCLOCKING
   b	Xil_Clockinit
#endif
   b	__main					; Jump to start

#else
   b	error					; Present exception level and selected exception level mismatch

#endif

InitEL1
#if (EL1_NONSECURE == 1)
   ldr	x1, =_vector_table			; Set vector table base address

   msr	VBAR_EL1, x1
   mrs	x0, CPACR_EL1
   mov	x2, #(0x3 << 0x20)
   bic	x0, x0, x2
   msr	CPACR_EL1, x0
   isb

;
; Clear FPUStatus variable to make sure that it contains current
; status of FPU i.e. disabled. In case of a warm restart execution
; when bss sections are not cleared, it may contain previously updated
; value which does not hold true now
;
#ifndef FREERTOS_BSP
   ldr	x0, =FPUStatus
   str	xzr, [x0]
#endif
   ldr	x2, =|Image$$ARM_LIB_STACK$$ZI$$Base|	; Define stack pointer for current exception level
   mov	sp, x2

; Disable MMU
   mov	x1, #0x0
   msr	SCTLR_EL1, x1
   isb

   TLBI	VMALLE1
   ic	IALLU					; Invalidate I cache to PoU
   bl	invalidate_dcaches
   dsb	sy
   isb

   ldr	x1, =MMUTableL0				; Get address of level 0 for TTBR0_EL1
   msr	TTBR0_EL1, x1				; Set TTBR0_EL1

;
; Set up memory attributes
; This equates to:
; 0 = b01000100 = Normal, Inner/Outer Non-Cacheable
; 1 = b11111111 = Normal, Inner/Outer WB/WA/RA
; 2 = b00000000 = Device-nGnRnE
; 3 = b00000100 = Device-nGnRE
; 4 = b10111011 = Normal, Inner/Outer WT/WA/RA
;
   ldr	x1, =0x000000BB0400FF44
   msr	MAIR_EL1, x1

#if defined (versal)
;
; Set up TCR_EL1
; Physical Address Size PS =  100 -> 44bits 16TB
; Granual Size TG0 = 00 -> 4KB
; size offset of the memory region T0SZ = 20 -> (region size 2^(64-20) = 2^44)
;
    ldr	x1,=0x485800514
#else
;
; Set up TCR_EL1
; Physical Address Size PS =  010 -> 40bits 1TB
; Granule Size TG0 = 00 -> 4KB
; Size offset of the memory region T0SZ = 24 -> (region size 2^(64-24) = 2^40)
;
   ldr	x1, =0x285800518
#endif
   msr	TCR_EL1, x1
   isb


; Enable SError Exception for asynchronous abort
   mrs	x1,DAIF
   mov	x2, #(0x1<<8)
   bic	x1,x1,x2
   msr	DAIF,x1


; Enable MMU
   mov	x1,#0x0
   orr	x1, x1, #(1 << 18)			; Set WFE non trapping
   orr	x1, x1, #(1 << 17)			; Set WFI non trapping
   orr	x1, x1, #(1 << 5)			; Set CP15 barrier enabled
   orr	x1, x1, #(1 << 12)			; Set I bit
   orr	x1, x1, #(1 << 2)			; Set C bit
   orr	x1, x1, #(1 << 0)			; Set M bit
   msr	SCTLR_EL1, x1
   isb

   bl	__main					; Jump to start
#else
   b	error					; present exception level and selected exception level mismatch
#endif

error
   b	error

invalidate_dcaches
   dmb	ISH
   mrs	x0, CLIDR_EL1				; x0 =  CLIDR
   ubfx	w2, w0, #24, #3				; w2 = CLIDR>Loc
   cmp	w2, #0					; LoC is 0?
   b.eq	invalidateCaches_end			; No cleaning required and enable MMU
   mov	w1, #0					; w1 = level iterator

invalidateCaches_flush_level
   add	w3, w1, w1, lsl #1			; w3 = w1 * 3 (right-shift for cache type)
   lsr	w3, w0, w3				; w3 = w0 >> w3
   ubfx	w3, w3, #0, #3				; w3 = cache type of this level
   cmp	w3, #2					; No cache at this level?
   b.lt	invalidateCaches_next_level
   lsl	w4, w1, #1
   msr	CSSELR_EL1, x4				; Select current cache level in CSSELR
   isb						; ISB required to reflect new CSIDR
   mrs	x4, CCSIDR_EL1				; w4 = CSIDR
   ubfx	w3, w4, #0, #3
   add	w3, w3, #2				; w3 = log2(line size)
   ubfx	w5, w4, #13, #15
   ubfx	w4, w4, #3, #10				; w4 = Way number
   clz	w6, w4					; w6 = 32 - log2(number of ways)

invalidateCaches_flush_set
   mov	w8, w4					; w8 = Way number

invalidateCaches_flush_way
   lsl	w7, w1, #1				; Fill level field
   lsl	w9, w5, w3
   orr	w7, w7, w9				; Fill index field
   lsl	w9, w8, w6
   orr	w7, w7, w9				; Fill way field
   dc	CISW, x7				; Invalidate by set/way to point of coherency
   subs	w8, w8, #1				; Decrement way
   b.ge	invalidateCaches_flush_way
   subs	w5, w5, #1				; Decrement set
   b.ge	invalidateCaches_flush_set

invalidateCaches_next_level
   add	w1, w1, #1				; Next level
   cmp	w2, w1
   b.gt	invalidateCaches_flush_level

invalidateCaches_end
   ret

   END
;
; @} End of "addtogroup a53_64_boot_code"
;
