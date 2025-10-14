/******************************************************************************
* Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file microblaze_selftest.S
*
* @addtogroup microblaze_pseudo_asm_macro
* @{
* <h2> microblaze_selftest.S </h2>
*
* This routine provides an internal self test of the MicroBlaze processor. The
* register file and all integer execution units are tested. Long instructions
* are also included if 64-bit mode is enabled. Currently FPU, MMU, BTC, data
* and instruction caches, and GET/PUT instructions are not covered.
*
* The routine is not reentrant, and disables interrupts and exceptions during
* execution. This can result in increased interrupt latency.
*
* Call this routine regularly from a timer interrupt.
*
* When called from C code the routine should be declared as:
*    extern int microblaze_selftest();
*
* @param	None.
*
* @return
*	- 0 if self test was successful
*	- Error code if self test failed:
*	    Bit 0:  Register file test failed
*	    Bit 1:  ALU test failed
*	    Bit 2:  Shift unit test failed
*	    Bit 3:  Load-store test failed
*           Bit 4:  Branch instruction test failed
*	    Bit 5:  Barrel shifter test failed
*	    Bit 6:  Multiplier test failed
*	    Bit 7:  Integer divide test failed
*	    Bit 8:  Pattern compare test failed
*	    Bit 9:  MSR instruction test failed
*	    Bit 10: Reorder instruction test failed
*
* @note
*    This routine assumes that the processor is in privileged mode when it is
*    called, if the MMU is enabled.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- -------- -----------------------------------------------
* 1.0   sa     09/07/20  First release
*
*
******************************************************************************/

#include "xparameters.h"
#include "microblaze_instructions.h"

/* Define if execution units are used */
#ifdef XPAR_MICROBLAZE_USE_BARREL
  #if XPAR_MICROBLAZE_USE_BARREL > 0
    #define USE_BARREL
  #endif
#endif

#ifdef XPAR_MICROBLAZE_USE_HW_MUL
  #if XPAR_MICROBLAZE_USE_HW_MUL > 0
    #define USE_HW_MUL
  #endif
  #if XPAR_MICROBLAZE_USE_HW_MUL == 2
    #define USE_HW_MUL_2
  #endif
#endif

#ifdef XPAR_MICROBLAZE_USE_DIV
  #if XPAR_MICROBLAZE_USE_DIV > 0
    #define USE_DIV
  #endif
#endif

#ifdef XPAR_MICROBLAZE_USE_PCMP_INSTR
  #if XPAR_MICROBLAZE_USE_PCMP_INSTR > 0
    #define USE_PCMP_INSTR
  #endif
#endif

#ifdef XPAR_MICROBLAZE_USE_MSR_INSTR
  #if XPAR_MICROBLAZE_USE_MSR_INSTR > 0
    #define USE_MSR_INSTR
  #endif
#endif

#ifdef XPAR_MICROBLAZE_USE_REORDER_INSTR
  #if XPAR_MICROBLAZE_USE_REORDER_INSTR > 0
    #define USE_REORDER_INSTR
  #endif
#endif

#if defined (__arch64__)
  #define OFFSET(index) ((index) * 8)
  #define INCR(index) addlik r1, (index) * 8
  #define DATA .quad
#else
  #define OFFSET(index) ((index) * 4)
  #define INCR(index) addik r1, r1, (index) * 4
  #define DATA .long
#endif


/*****************************************************************************
*       Machine Status Register masks
******************************************************************************/

#define MSR_CARRY_MASK 0x80000004
#define MSR_EE_IE_MASK 0x00000102

#define MSR_PVR_BIT_MASK 0x00000000
#ifdef XPAR_MICROBLAZE_PVR
  #if XPAR_MICROBLAZE_PVR > 0
    #undef MSR_PVR_BIT_MASK
    #define MSR_PVR_BIT_MASK 0x00000400
  #endif
#endif

#define MSR_BIT_MASK (0x8000000E | MSR_PVR_BIT_MASK)


/*****************************************************************************
*       Test macros
******************************************************************************/

#define TEST_ALU_ARITH(instr, load, offset)        \
	instr	r5,  r2, r4;                       \
	load	r6,  r1, OFFSET(offset);           \
	CMPU	r7,  r5, r6;                       \
	BNEID	r7,  L_microblaze_selftest_aluerr; \
	lwi 	r8,  r1, OFFSET(offset + 1);       \
	mfs	r9,  rmsr;                         \
	andi	r9,  r9, MSR_CARRY_MASK;           \
	cmpu	r9,  r9, r8;                       \
	BNEID	r9,  L_microblaze_selftest_aluerr

#define TEST_ALU_LOGIC(instr, load, offset)        \
	instr	r5,  r2, r4;                       \
	load	r6,  r1, OFFSET(offset);           \
	CMPU	r7,  r5, r6;                       \
	BNEID	r7,  L_microblaze_selftest_aluerr

#define TEST_SHIFT(instr, load, offset)              \
	instr	r5,  r2;                             \
	load	r6,  r1, OFFSET(offset);             \
	CMPU	r7,  r5, r6;                         \
	BNEID	r7,  L_microblaze_selftest_shifterr

#define TEST_SHIFTC(instr, load, offset)             \
	instr	r5,  r2;                             \
	load	r6,  r1, OFFSET(offset);             \
	CMPU	r7,  r5, r6;                         \
	BNEID	r7,  L_microblaze_selftest_shifterr; \
	lwi 	r8,  r1, OFFSET(offset + 1);         \
	mfs	r9,  rmsr;                           \
	andi	r9,  r9, MSR_CARRY_MASK;             \
	cmpu	r9,  r9, r8;                         \
	BNEID	r9,  L_microblaze_selftest_shifterr

#define TEST_BARREL(instr, load, offset)             \
	instr	r5,  r2, r4;                         \
	load	r6,  r1, OFFSET(offset);             \
	CMPU	r7,  r5, r6;                         \
	BNEID	r7,  L_microblaze_selftest_barrelerr

#define TEST_BARREL_IMM(instr, immw, imms, load, offset) \
	ori	r5,  r2, 0;                              \
	instr	r5,  r4, immw, imms;                     \
	load	r6,  r1, OFFSET(offset);                 \
	CMPU	r7,  r5, r6;                             \
	BNEID	r7,  L_microblaze_selftest_barrelerr

#define TEST_MUL(instr, load, byteoffset)            \
	instr	r5,  r2, r4;                         \
	load	r6,  r1, byteoffset;                 \
	CMPU	r7,  r5, r6;                         \
	BNEID	r7,  L_microblaze_selftest_mulerr

#define TEST_DIV(instr, load, byteoffset)            \
	instr	r5,  r2, r4;                         \
	load	r6,  r1, byteoffset;                 \
	CMPU	r7,  r5, r6;                         \
	BNEID	r7,  L_microblaze_selftest_diverr

#define TEST_PCMP(instr, load, offset)               \
	instr	r5,  r2, r4;                         \
	load	r6,  r1, OFFSET(offset);             \
	CMPU	r7,  r5, r6;                         \
	BNEID	r7,  L_microblaze_selftest_pcmperr


	.text
	.globl	microblaze_selftest
	.ent	microblaze_selftest
	.align	2

microblaze_selftest:

/*****************************************************************************
*       Preamble
******************************************************************************/

	/* Disable interrupts and exceptions, save MSR */
#ifdef USE_MSR_INSTR
	msrclr	r3, MSR_EE_IE_MASK
#else
	mfs	r3, rmsr
	andi	r3, r3, ~MSR_EE_IE_MASK
	mts	rmsr, r3
#endif
	swi	r3, r0, L_microblaze_selftest_regs

	/* Save registers, except r3 temporary register and return value */
	ADDIK	r3,  r0, L_microblaze_selftest_regs
	SI	r1,  r3, OFFSET(1)
	SI	r2,  r3, OFFSET(2)
	SI	r4,  r3, OFFSET(4)
	SI	r5,  r3, OFFSET(5)
	SI	r6,  r3, OFFSET(6)
	SI	r7,  r3, OFFSET(7)
	SI	r8,  r3, OFFSET(8)
	SI	r9,  r3, OFFSET(9)
	SI	r10, r3, OFFSET(10)
	SI	r11, r3, OFFSET(11)
	SI	r12, r3, OFFSET(12)
	SI	r13, r3, OFFSET(13)
	SI	r14, r3, OFFSET(14)
	SI	r15, r3, OFFSET(15)
	SI	r16, r3, OFFSET(16)
	SI	r17, r3, OFFSET(17)
	SI	r18, r3, OFFSET(18)
	SI	r19, r3, OFFSET(19)
	SI	r20, r3, OFFSET(20)
	SI	r21, r3, OFFSET(21)
	SI	r22, r3, OFFSET(22)
	SI	r23, r3, OFFSET(23)
	SI	r24, r3, OFFSET(24)
	SI	r25, r3, OFFSET(25)
	SI	r26, r3, OFFSET(26)
	SI	r27, r3, OFFSET(27)
	SI	r28, r3, OFFSET(28)
	SI	r29, r3, OFFSET(29)
	SI	r30, r3, OFFSET(30)
	SI	r31, r3, OFFSET(31)

	/* Set r3 to test pass = 0 */
	ADDIK	r3, r0, 0

/*****************************************************************************
*       1. Test register file
******************************************************************************/
L_microblaze_selftest_reg:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_regdata

L_microblaze_selftest_regloop:
	/* Load test pattern into r2, r4 - r31 */
	LI	r2,  r1, 0
	LI	r4,  r1, 0
	LI	r5,  r1, 0
	LI	r6,  r1, 0
	LI	r7,  r1, 0
	LI	r8,  r1, 0
	LI	r9,  r1, 0
	LI	r10, r1, 0
	LI	r11, r1, 0
	LI	r12, r1, 0
	LI	r13, r1, 0
	LI	r14, r1, 0
	LI	r15, r1, 0
	LI	r16, r1, 0
	LI	r17, r1, 0
	LI	r18, r1, 0
	LI	r19, r1, 0
	LI	r20, r1, 0
	LI	r21, r1, 0
	LI	r22, r1, 0
	LI	r23, r1, 0
	LI	r24, r1, 0
	LI	r25, r1, 0
	LI	r26, r1, 0
	LI	r27, r1, 0
	LI	r28, r1, 0
	LI	r29, r1, 0
	LI	r30, r1, 0
	LI	r31, r1, 0

	/* Compare registers pair by pair */
	CMPU	r2,  r2, r4
	BNEID	r2,  L_microblaze_selftest_regerr
	CMPU	r4,  r4, r5
	BNEID	r4,  L_microblaze_selftest_regerr
	CMPU	r5,  r5, r6
	BNEID	r5,  L_microblaze_selftest_regerr
	CMPU	r6,  r6, r7
	BNEID	r6,  L_microblaze_selftest_regerr
	CMPU	r7,  r7, r8
	BNEID	r7,  L_microblaze_selftest_regerr
	CMPU	r8,  r8, r9
	BNEID	r8,  L_microblaze_selftest_regerr
	CMPU	r9,  r9, r10
	BNEID	r9,  L_microblaze_selftest_regerr
	CMPU	r10, r10, r11
	BNEID	r10, L_microblaze_selftest_regerr
	CMPU	r11, r11, r12
	BNEID	r11, L_microblaze_selftest_regerr
	CMPU	r12, r12, r13
	BNEID	r12, L_microblaze_selftest_regerr
	CMPU	r13, r13, r14
	BNEID	r13, L_microblaze_selftest_regerr
	CMPU	r14, r14, r15
	BNEID	r14, L_microblaze_selftest_regerr
	CMPU	r15, r15, r16
	BNEID	r15, L_microblaze_selftest_regerr
	CMPU	r16, r16, r17
	BNEID	r16, L_microblaze_selftest_regerr
	CMPU	r17, r17, r18
	BNEID	r17, L_microblaze_selftest_regerr
	CMPU	r18, r18, r19
	BNEID	r18, L_microblaze_selftest_regerr
	CMPU	r19, r19, r20
	BNEID	r19, L_microblaze_selftest_regerr
	CMPU	r20, r20, r21
	BNEID	r20, L_microblaze_selftest_regerr
	CMPU	r21, r21, r22
	BNEID	r21, L_microblaze_selftest_regerr
	CMPU	r22, r22, r23
	BNEID	r22, L_microblaze_selftest_regerr
	CMPU	r23, r23, r24
	BNEID	r23, L_microblaze_selftest_regerr
	CMPU	r24, r24, r25
	BNEID	r24, L_microblaze_selftest_regerr
	CMPU	r25, r25, r26
	BNEID	r25, L_microblaze_selftest_regerr
	CMPU	r26, r26, r27
	BNEID	r26, L_microblaze_selftest_regerr
	CMPU	r27, r27, r28
	BNEID	r27, L_microblaze_selftest_regerr
	CMPU	r28, r28, r29
	BNEID	r28, L_microblaze_selftest_regerr
	CMPU	r29, r29, r30
	BNEID	r29, L_microblaze_selftest_regerr
	CMPU	r30, r30, r31
	BEQI 	r30, L_microblaze_selftest_regnext

L_microblaze_selftest_regerr:
	/* Test error - set register file error (bit 0) */
	ori	r3,  r3, 1

L_microblaze_selftest_regnext:
	/* Loop back with next test pattern - end after zero pattern */
	BNEID	r31, L_microblaze_selftest_regloop
	INCR(1)


/*****************************************************************************
*       2. Test ALU
******************************************************************************/
L_microblaze_selftest_alu:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_aludata

L_microblaze_selftest_aluloop:
	/* Load operands into r2, r4 */
	LI	r2,  r1, OFFSET(0)
	LI	r4,  r1, OFFSET(1)

	/* Execute all ALU arithmetic instructions */
	TEST_ALU_ARITH(add,    lwi,  2)
	TEST_ALU_ARITH(addc,   lwi,  4)
	TEST_ALU_ARITH(rsub,   lwi,  6)
	TEST_ALU_ARITH(rsubc,  lwi,  8)
	TEST_ALU_LOGIC(cmp,    lwi, 10)

	/* Execute all ALU logical instructions */
	TEST_ALU_LOGIC(and,  lwi, 11)
	TEST_ALU_LOGIC(andn, lwi, 12)
	TEST_ALU_LOGIC(or,   lwi, 13)
	TEST_ALU_LOGIC(xor,  lwi, 14)

	nop
	bri	L_microblaze_selftest_alunext

L_microblaze_selftest_aluerr:
	/* Test error - set ALU error (bit 1) */
	ori	r3, r3, 2

L_microblaze_selftest_alunext:
	/* Loop back with next test data - end after zero data */
	BNEID	r2, L_microblaze_selftest_aluloop
	INCR(15)


/*****************************************************************************
*       3. Test shift unit
******************************************************************************/
L_microblaze_selftest_shift:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_shiftdata

L_microblaze_selftest_shiftloop:
	/* Load operand into r2 */
	LI	r2,  r1, OFFSET(0)

	/* Test instructions clz, sext8, sext16, sra, src, srl */
	TEST_SHIFT(clz,     lwi,  1)
	TEST_SHIFT(sext8,   lwi,  2)
	TEST_SHIFT(sext16,  lwi,  3)

	TEST_SHIFTC(sra,    lwi,  4)
	TEST_SHIFTC(src,    lwi,  6)
	TEST_SHIFTC(srl,    lwi,  8)

	nop
	bri	L_microblaze_selftest_shiftnext

L_microblaze_selftest_shifterr:
	/* Test error - set shift logic error (bit 2) */
	ori	r3, r3, 4

L_microblaze_selftest_shiftnext:
	/* Loop back with next test data - end after zero data */
	BNEID	r2, L_microblaze_selftest_shiftloop
	INCR(10)


/*****************************************************************************
*       4. Test load-store
******************************************************************************/
L_microblaze_selftest_ldst:
	/* Get pointer to test data */
	ADDIK	r1,   r0, L_microblaze_selftest_ldstdata

	/* Set static address and offsets */
	ADDIK	r4,   r0, L_microblaze_selftest_ldstptr
	ADDIK	r24,  r0, 4
	ADDIK	r28,  r0, 8

L_microblaze_selftest_ldstloop:
	/* Load operand into r2 */
	LI	r2,  r1, OFFSET(0)

	/* Test instructions lbu, lhu, lw, sb, sh, sw */
	sb	r2,  r4,  r0
	sh	r2,  r4,  r24
	sw	r2,  r4,  r28
	lbu	r8,  r4,  r0
	lhu	r9,  r4,  r24
	lw	r10, r4,  r28

	/* Compare load results */
	ANDI	r11, r2,  0xFF
	ANDI	r12, r2,  0xFFFF
	CMPU	r16, r11, r8
	BNEID	r16, L_microblaze_selftest_ldsterr
	CMPU	r17, r12, r9
	BNEID	r17, L_microblaze_selftest_ldsterr
	CMPU	r18, r2,  r10
	BEQI 	r18, L_microblaze_selftest_ldstnext

L_microblaze_selftest_ldsterr:
	/* Test error - set load-store error (bit 3) */
	ori	r3, r3, 8

L_microblaze_selftest_ldstnext:
	/* Loop back with next test data - end after zero data */
	BNEID	r2, L_microblaze_selftest_ldstloop
	INCR(1)


/*****************************************************************************
*       4. Test branch
******************************************************************************/
L_microblaze_selftest_branch:
	/* Test instructions blt, ble, bge, bgt, br */
	/* Untested instructions: rtbd, rtid, rted, brk */
	addik	r2, r0, 0	/* EQ */
	addik	r4, r0, 1	/* GT */
	addik	r5, r0, -1	/* LT */

	/* Taken branches */
	BEQI	r2, L_microblaze_selftest_brancheq
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_brancheq:
	BGEI	r2, L_microblaze_selftest_branchge0
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_branchge0:
	BGEI	r4, L_microblaze_selftest_branchge1
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_branchge1:
	BGTI	r4, L_microblaze_selftest_branchgt
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_branchgt:
	BLEI	r2, L_microblaze_selftest_branchle0
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_branchle0:
	BLEI	r5, L_microblaze_selftest_branchle1
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_branchle1:
	BLTI	r5, L_microblaze_selftest_branchlt
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_branchlt:
	BNEI	r4, L_microblaze_selftest_branchne
	ori	r3, r3, 16	/* Test error */
L_microblaze_selftest_branchne:

	/* Not taken branches */
	BEQI	r4, L_microblaze_selftest_brancherr
	BEQI	r5, L_microblaze_selftest_brancherr
	BGEI	r5, L_microblaze_selftest_brancherr
	BGTI	r2, L_microblaze_selftest_brancherr
	BGTI	r5, L_microblaze_selftest_brancherr
	BLEI	r4, L_microblaze_selftest_brancherr
	BLTI	r2, L_microblaze_selftest_brancherr
	BLTI	r4, L_microblaze_selftest_brancherr
	BNEI	r2, L_microblaze_selftest_brancherr

	BRI	L_microblaze_selftest_branchnext

L_microblaze_selftest_brancherr:
	/* Test error - set branch error (bit 4) */
	ori	r3, r3, 16

L_microblaze_selftest_branchnext:


/*****************************************************************************
*       6. Test barrel shifter
******************************************************************************/
#ifdef USE_BARREL
L_microblaze_selftest_barrel:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_barreldata

L_microblaze_selftest_barrelloop:
	/* Load operands into r2, r4 */
	LI	r2,  r1, OFFSET(0)
	LI	r4,  r1, OFFSET(1)

	/* Test instructions bsrl, bsra, bsll, bsifi, bsefi */
	TEST_BARREL(bsrl, lwi,  2)
	TEST_BARREL(bsra, lwi,  3)
	TEST_BARREL(bsll, lwi,  4)

	TEST_BARREL_IMM(bsifi, 11, 6, lwi, 5);
	TEST_BARREL_IMM(bsefi,  6, 1, lwi, 6);

	nop
	bri	L_microblaze_selftest_barrelnext

L_microblaze_selftest_barrelerr:
	/* Test error - set barrel logic error (bit 5) */
	ori	r3, r3, 32

L_microblaze_selftest_barrelnext:
	/* Loop back with next test data - end after zero data */
	BNEID	r2, L_microblaze_selftest_barrelloop
	INCR(7)
#endif

/*****************************************************************************
*       7. Test multiplier
******************************************************************************/
#ifdef USE_HW_MUL
L_microblaze_selftest_mul:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_muldata

L_microblaze_selftest_mulloop:
	/* Load operands into r2, r4 */
	lwi	r2,  r1, 0
	lwi	r4,  r1, 4

	/* Test instruction mul */
	TEST_MUL(mul,  lwi,  8)
#ifdef USE_HW_MUL_2
	/* Test instructions mulh, mulhu, mulhsu */
	TEST_MUL(mulh,   lwi, 12)
	TEST_MUL(mulhu,  lwi, 16)
	TEST_MUL(mulhsu, lwi, 20)
#endif
	nop
	bri	L_microblaze_selftest_mulnext

L_microblaze_selftest_mulerr:
	/* Test error - set multiply error (bit 6) */
	ori	r3, r3, 64

L_microblaze_selftest_mulnext:
	/* Loop back with next test data - end after zero data */
	BNEID	r2, L_microblaze_selftest_mulloop
#ifdef USE_HW_MUL_2
	addik	r1, r1, 24
#else
	addik	r1, r1, 12
#endif
#endif


/*****************************************************************************
*       8. Test integer divide
******************************************************************************/
#ifdef USE_DIV
L_microblaze_selftest_div:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_divdata

L_microblaze_selftest_divloop:
	/* Load operands into r2, r4 */
	lwi	r2,  r1, 0
	lwi	r4,  r1, 4

	/* Test instructions idiv, idivu */
	TEST_DIV(idiv,  lwi,  8)
	TEST_DIV(idivu, lwi,  12)

	nop
	bri	L_microblaze_selftest_divnext

L_microblaze_selftest_diverr:
	/* Test error - set integer divide error (bit 7) */
	ori	r3, r3, 128

L_microblaze_selftest_divnext:
	/* Loop back with next test data - end after zero data */
	BNEID	r4, L_microblaze_selftest_divloop
	addik	r1, r1, 16
#endif

/*****************************************************************************
*      	9. Test pattern compare
******************************************************************************/
#ifdef USE_PCMP_INSTR
L_microblaze_selftest_pcmp:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_pcmpdata

L_microblaze_selftest_pcmploop:
	/* Load operands into r2, r4 */
	LI	r2,  r1, OFFSET(0)
	LI	r4,  r1, OFFSET(1)

	/* Test instructions pcmpbf, pcmpeq, pcmpne */
	/* 64-bit: add instructions pcmplbf, pcmpleq, pcmplne */
	TEST_PCMP(pcmpbf, lwi,  2)
	TEST_PCMP(pcmpeq, lwi,  3)
	TEST_PCMP(pcmpne, lwi,  4)

	nop
	bri	L_microblaze_selftest_pcmpnext

L_microblaze_selftest_pcmperr:
	/* Test error - set pcmp logic error (bit 8) */
	ori	r3, r3, 256

L_microblaze_selftest_pcmpnext:
	/* Loop back with next test data - end after zero data */
	BNEID	r2, L_microblaze_selftest_pcmploop
	INCR(5)
#endif

/*****************************************************************************
*      10. Test MSR instructions
******************************************************************************/
L_microblaze_selftest_msr:
#ifdef USE_MSR_INSTR
	/* Test instructions msrclr, msrset, mfs msr */
	msrset	r0,  ~MSR_EE_IE_MASK & 0x7FFF
	nop
	mfs	r6,  rmsr
	nop
	msrclr	r7,  0x7FF
	nop
	mfs	r8,  rmsr
	nop
#endif
	/* Test instructions mfs msr, mts msr */
        /* Bits not included: ICE, DZO, DCE, EE, EIP, UMS, VMS */
	addik	r9,   r0, MSR_BIT_MASK & ~MSR_EE_IE_MASK
	mts	rmsr, r9
	nop
	mfs	r10, rmsr

	/* Compare instruction result */
#ifdef USE_MSR_INSTR
	addik	r11, r0, MSR_PVR_BIT_MASK
	cmpu	r12, r6, r7
	BNEID	r12, L_microblaze_selftest_msrerr
	cmpu	r13, r8, r11
	BNEID 	r13, L_microblaze_selftest_msrerr
#endif
	cmpu	r14, r9, r10
	BEQI 	r14, L_microblaze_selftest_msrnext

L_microblaze_selftest_msrerr:
	/* Test error - set MSR instruction error (bit 9) */
	ori	r3, r3, 512

L_microblaze_selftest_msrnext:


/*****************************************************************************
*      11. Test reorder instructions
******************************************************************************/
#ifdef USE_REORDER_INSTR
L_microblaze_selftest_reorder:
	/* Get pointer to test data */
	ADDIK	r1,  r0, L_microblaze_selftest_reorderdata

	/* Set static address and offsets */
	ADDIK	r4,   r0, L_microblaze_selftest_ldstptr
	ADDIK	r24,  r0, 4
	ADDIK	r28,  r0, 8

L_microblaze_selftest_reorderloop:
	/* Load operand into r2 */
	LI	r2,  r1, OFFSET(0)

	/* Test instructions swapb, swaph, lbur, lhur, lwr, sbr, shr, swr */
	sbr	r2,  r4, r0
	shr	r2,  r4, r24
	swr	r2,  r4, r28
	lbur	r8,  r4, r0
	lhur	r9,  r4, r24
	lwr	r10, r4, r28
	swapb	r21, r2
	swaph	r22, r2

	/* Load swap results */
	LI	r13,  r1, OFFSET(1)
	LI	r14,  r1, OFFSET(2)

	/* Compare load results */
	ANDI	r11, r2,  0xFF
	ANDI	r12, r2,  0xFFFF
	CMPU	r16, r11, r8
	BNEID	r16, L_microblaze_selftest_reordererr
	CMPU	r17, r12, r9
	BNEID	r17, L_microblaze_selftest_reordererr
	CMPU	r18, r2, r10
	BNEID	r18, L_microblaze_selftest_reordererr
	CMPU	r19, r21, r13
	BNEID	r19, L_microblaze_selftest_reordererr
	CMPU	r20, r22, r14
	BEQI 	r20, L_microblaze_selftest_reordernext

L_microblaze_selftest_reordererr:
	/* Test error - set reorder instruction error (bit 10) */
	ori	r3, r3, 1024

L_microblaze_selftest_reordernext:
	/* Loop back with next test data - end after zero data */
	BNEID	r2, L_microblaze_selftest_reorderloop
	INCR(3)
#endif


/*****************************************************************************
*      Postamble
******************************************************************************/

	/* Restore all registers, except r3 return value and r4 temporary */
	ADDIK	r31, r0,  L_microblaze_selftest_regs
	LI	r1,  r31, OFFSET(1)
	LI	r2,  r31, OFFSET(2)
	LI	r4,  r31, OFFSET(4)
	LI	r5,  r31, OFFSET(5)
	LI	r6,  r31, OFFSET(6)
	LI	r7,  r31, OFFSET(7)
	LI	r8,  r31, OFFSET(8)
	LI	r9,  r31, OFFSET(9)
	LI	r10, r31, OFFSET(10)
	LI	r11, r31, OFFSET(11)
	LI	r12, r31, OFFSET(12)
	LI	r13, r31, OFFSET(13)
	LI	r14, r31, OFFSET(14)
	LI	r15, r31, OFFSET(15)
	LI	r16, r31, OFFSET(16)
	LI	r17, r31, OFFSET(17)
	LI	r18, r31, OFFSET(18)
	LI	r19, r31, OFFSET(19)
	LI	r20, r31, OFFSET(20)
	LI	r21, r31, OFFSET(21)
	LI	r22, r31, OFFSET(22)
	LI	r23, r31, OFFSET(23)
	LI	r24, r31, OFFSET(24)
	LI	r25, r31, OFFSET(25)
	LI	r26, r31, OFFSET(26)
	LI	r27, r31, OFFSET(27)
	LI	r28, r31, OFFSET(28)
	LI	r29, r31, OFFSET(29)
	LI	r30, r31, OFFSET(30)
	LI	r31, r31, OFFSET(31)

	/* Restore MSR */
	lwi	r4, r0, L_microblaze_selftest_regs
	mts	rmsr, r4

	/* Return */
	rtsd	r15, 8
	nop
L_microblaze_selftest_end:
	.end	microblaze_selftest

/*****************************************************************************
*      Temporary storage used by this routine
*      MSR followd by registers r1 - r31
******************************************************************************/
	.data
	.align	3
L_microblaze_selftest_regs:
	.rept	32
#if defined (__arch64__)
	.quad	0
#else
	.long	0
#endif
	.endr

/*****************************************************************************
*      Test data used by this routine
******************************************************************************/
L_microblaze_selftest_regdata:
#if defined (__arch64__)
	.quad	0x5555555555555555
	.quad	0xAAAAAAAAAAAAAAAA
	.quad	0xFFFFFFFFFFFFFFFF
	.quad	0x0000000000000000
#else
	.long	0x55555555
	.long	0xAAAAAAAA
	.long	0xFFFFFFFF
	.long	0x00000000
#endif

L_microblaze_selftest_aludata:
	DATA	0x55555555	/* rA operand       */
	DATA	0x55555556      /* rB operand       */
	DATA	0xAAAAAAAB	/* rD result: add   */
	DATA	0x00000000	/* MSR.C:     add   */
	DATA	0xAAAAAAAB	/* rD result: addc  */
	DATA	0x00000000	/* MSR.C:     addc  */
	DATA	0x00000001	/* rD result: rsub  */
	DATA	MSR_CARRY_MASK	/* MSR.C:     rsub  */
	DATA	0x00000001	/* rD result: rsubc */
	DATA	MSR_CARRY_MASK	/* MSR.C:     rsubc */
	DATA	0x00000001	/* rD result: cmp   */
	DATA	0x55555554	/* rD result: and   */
	DATA	0x00000001	/* rD result: andn  */
	DATA	0x55555557	/* rD result: or    */
	DATA	0x00000003	/* rD result: xor   */

	DATA	0xAAAAAAAB	/* rA operand       */
	DATA	0xAAAAAAAA      /* rB operand       */
	DATA	0x55555555	/* rD result: add   */
	DATA	MSR_CARRY_MASK	/* MSR.C:     add   */
	DATA	0x55555556	/* rD result: addc  */
	DATA	MSR_CARRY_MASK	/* MSR.C:     addc  */
	DATA	0xFFFFFFFF	/* rD result: rsub  */
	DATA	0x00000000     	/* MSR.C:     rsub  */
	DATA	0xFFFFFFFE	/* rD result: rsubc */
	DATA	0x00000000    	/* MSR.C:     rsubc */
	DATA	0xFFFFFFFF	/* rD result: cmp   */
	DATA	0xAAAAAAAA	/* rD result: and   */
	DATA	0x00000001	/* rD result: andn  */
	DATA	0xAAAAAAAB	/* rD result: or    */
	DATA	0x00000001	/* rD result: xor   */

	DATA	0xFFFFFFFF	/* rA operand       */
	DATA	0xFFFFFFFF      /* rB operand       */
	DATA	0xFFFFFFFE	/* rD result: add   */
	DATA	MSR_CARRY_MASK	/* MSR.C:     add   */
	DATA	0xFFFFFFFF	/* rD result: addc  */
	DATA	MSR_CARRY_MASK	/* MSR.C:     addc  */
	DATA	0x00000000 	/* rD result: rsub  */
	DATA	MSR_CARRY_MASK	/* MSR.C:     rsub  */
	DATA	0x00000000	/* rD result: rsubc */
	DATA	MSR_CARRY_MASK	/* MSR.C:     rsubc */
	DATA	0x00000000	/* rD result: cmp   */
	DATA	0xFFFFFFFF	/* rD result: and   */
	DATA	0x00000000	/* rD result: andn  */
	DATA	0xFFFFFFFF	/* rD result: or    */
	DATA	0x00000000	/* rD result: xor   */

	DATA	0x00000000	/* rA operand       */
	DATA	0xFFFFFFFF      /* rB operand       */
	DATA	0xFFFFFFFF	/* rD result: add   */
	DATA	0x00000000	/* MSR.C:     add   */
	DATA	0xFFFFFFFF	/* rD result: addc  */
	DATA	0x00000000	/* MSR.C:     addc  */
	DATA	0xFFFFFFFF	/* rD result: rsub  */
	DATA	MSR_CARRY_MASK	/* MSR.C:     rsub  */
	DATA	0xFFFFFFFF	/* rD result: rsubc */
	DATA	MSR_CARRY_MASK	/* MSR.C:     rsubc */
	DATA	0xFFFFFFFF	/* rD result: cmp   */
	DATA	0x00000000	/* rD result: and   */
	DATA	0x00000000	/* rD result: andn  */
	DATA	0xFFFFFFFF	/* rD result: or    */
	DATA	0xFFFFFFFF	/* rD result: xor   */

L_microblaze_selftest_shiftdata:
	DATA	0x55555555	/* rA operand        */
	DATA	0x00000001	/* rD result: clz    */
	DATA	0x00000055	/* rD result: sext8  */
	DATA	0x00005555	/* rD result: sext16 */
	DATA	0x2AAAAAAA	/* rD result: sra    */
	DATA	MSR_CARRY_MASK	/* MSR.C:     sra    */
	DATA	0xAAAAAAAA	/* rD result: src    */
	DATA	MSR_CARRY_MASK	/* MSR.C:     src    */
	DATA	0x2AAAAAAA	/* rD result: srl    */
	DATA	MSR_CARRY_MASK	/* MSR.C:     src    */

	DATA	0xAAAAAAAA	/* rA operand        */
	DATA	0x00000000	/* rD result: clz    */
	DATA	0xFFFFFFAA	/* rD result: sext8  */
	DATA	0xFFFFAAAA	/* rD result: sext16 */
	DATA	0xD5555555	/* rD result: sra    */
	DATA	0x00000000	/* MSR.C:     sra    */
	DATA	0x55555555	/* rD result: src    */
	DATA	0x00000000	/* MSR.C:     src    */
	DATA	0x55555555	/* rD result: srl    */
	DATA	0x00000000	/* MSR.C:     srl    */

	DATA	0xFFFFFFFF	/* rA operand        */
	DATA	0x00000000	/* rD result: clz    */
	DATA	0xFFFFFFFF	/* rD result: sext8  */
	DATA	0xFFFFFFFF	/* rD result: sext16 */
	DATA	0xFFFFFFFF	/* rD result: sra    */
	DATA	MSR_CARRY_MASK	/* MSR.C:     sra    */
	DATA	0xFFFFFFFF	/* rD result: src    */
	DATA	MSR_CARRY_MASK	/* MSR.C:     src    */
	DATA	0x7FFFFFFF	/* rD result: srl    */
	DATA	MSR_CARRY_MASK	/* MSR.C:     srl    */

	DATA	0x00000000	/* rA operand        */
	DATA	0x00000020	/* rD result: clz    */
	DATA	0x00000000	/* rD result: sext8  */
	DATA	0x00000000	/* rD result: sext16 */
	DATA	0x00000000	/* rD result: sra    */
	DATA	0x00000000	/* MSR.C:     sra    */
	DATA	0x00000000	/* rD result: src    */
	DATA	0x00000000	/* MSR.C:     src    */
	DATA	0x00000000	/* rD result: srl    */
	DATA	0x00000000	/* MSR.C:     srl    */

L_microblaze_selftest_ldstdata:
	DATA	0xDEADBEEF	/* rD operand         */
	DATA	0x12345678	/* rD operand         */
	DATA	0x00000000	/* rD operand         */
L_microblaze_selftest_ldstptr:
	DATA	0
	DATA	0
	DATA	0

L_microblaze_selftest_barreldata:
	DATA	0x55555555	/* rA operand         */
	DATA	16		/* rB operand         */
	DATA	0x00005555	/* rD result: bsrl    */
	DATA	0x00005555	/* rD result: bsra    */
	DATA	0x55550000	/* rD result: bsll    */
	DATA	0x55540415  	/* rD result: bsifi   */
	DATA	0x00000008     	/* rD result: bsefi   */

	DATA	0xAAAAAAAA	/* rA operand         */
	DATA	1		/* rB operand         */
	DATA	0x55555555	/* rD result: bsrl    */
	DATA	0xD5555555	/* rD result: bsra    */
	DATA	0x55555554	/* rD result: bsll    */
	DATA	0xAAAA006A  	/* rD result: bsifi   */
	DATA	0x00000000    	/* rD result: bsefi   */

	DATA	0xFFFFFFFF	/* rA operand         */
	DATA	25		/* rB operand         */
	DATA	0x0000007F	/* rD result: bsrl    */
	DATA	0xFFFFFFFF	/* rD result: bsra    */
	DATA	0xFE000000	/* rD result: bsll    */
	DATA	0xFFFE067F     	/* rD result: bsifi   */
	DATA	0x0000000C    	/* rD result: bsefi   */

	DATA	0x00000000	/* rA operand         */
	DATA	10		/* rB operand         */
	DATA	0x00000000	/* rD result: bsrl    */
	DATA	0x00000000	/* rD result: bsra    */
	DATA	0x00000000	/* rD result: bsll    */
	DATA	0x00000280  	/* rD result: bsifi   */
	DATA	0x00000005    	/* rD result: bsefi   */

L_microblaze_selftest_muldata:
	.long	0x55555555	/* rA operand         */
	.long	0x55555555	/* rB operand         */
	.long	0x38E38E39	/* rD result: mul     */
#ifdef USE_HW_MUL_2
	.long	0x1C71C71C	/* rD result: mulh    */
	.long	0x1C71C71C	/* rD result: mulhu   */
	.long	0x1C71C71C	/* rD result: mulhsu  */
#endif
	.long	0xAAAAAAAA	/* rA operand         */
	.long	0xAAAAAAAA	/* rB operand         */
	.long	0xE38E38E4	/* rD result: mul     */
#ifdef USE_HW_MUL_2
	.long	0x1C71C71C	/* rD result: mulh    */
	.long	0x71C71C70	/* rD result: mulhu   */
	.long	0xC71C71C6	/* rD result: mulhsu  */
#endif
	.long	0xFFFFFFFF	/* rA operand         */
	.long	0xFFFFFFFF	/* rB operand         */
	.long	0x00000001	/* rD result: mul     */
#ifdef USE_HW_MUL_2
	.long	0x00000000	/* rD result: mulh    */
	.long	0xFFFFFFFE	/* rD result: mulhu   */
	.long	0xFFFFFFFF	/* rD result: mulhsu  */
#endif
	.long	0x00000000	/* rA operand         */
	.long	0x00000000	/* rB operand         */
	.long	0x00000000	/* rD result: mul     */
#ifdef USE_HW_MUL_2
	.long	0x00000000	/* rD result: mulh    */
	.long	0x00000000	/* rD result: mulhu   */
	.long	0x00000000	/* rD result: mulhsu  */
#endif

L_microblaze_selftest_divdata:
	.long	0x55555555	/* rA operand         */
	.long	0x55555555	/* rB operand         */
	.long	0x00000001	/* rD result: div     */
	.long	0x00000001	/* rD result: divu    */

	.long	0xAAAAAAAA	/* rA operand         */
	.long	0xAAAAAAAA	/* rB operand         */
	.long	0x00000001	/* rD result: div     */
	.long	0x00000001	/* rD result: divu    */

	.long	0xFFFFFFFF	/* rA operand         */
	.long	0xFFFFFFFF	/* rB operand         */
	.long	0x00000001	/* rD result: div     */
	.long	0x00000001	/* rD result: divu    */

	.long	0x00000001	/* rA operand         */
	.long	0x00000000	/* rB operand         */
	.long	0x00000000	/* rD result: div     */
	.long	0x00000000	/* rD result: divu    */

	.align	3
L_microblaze_selftest_pcmpdata:
	DATA	0x55555555	/* rA operand         */
	DATA	0x44445555	/* rB operand         */
	DATA	0x00000003	/* rD result: pcmpbf  */
	DATA	0x00000000	/* rD result: pcmpeq  */
	DATA	0x00000001	/* rD result: pcmpne  */

	DATA	0xAAAAAAAA	/* rA operand         */
	DATA	0xAAAAAAAA	/* rB operand         */
	DATA	0x00000001	/* rD result: pcmpbf  */
	DATA	0x00000001	/* rD result: pcmpeq  */
	DATA	0x00000000	/* rD result: pcmpne  */

	DATA	0xFFFFFFFF	/* rA operand         */
	DATA	0x000000FF	/* rB operand         */
	DATA	0x00000004	/* rD result: pcmpbf  */
	DATA	0x00000000	/* rD result: pcmpeq  */
	DATA	0x00000001	/* rD result: pcmpne  */

	DATA	0x00000000	/* rA operand         */
	DATA	0xDEADBEEF	/* rB operand         */
	DATA	0x00000000	/* rD result: pcmpbf  */
	DATA	0x00000000	/* rD result: pcmpeq  */
	DATA	0x00000001	/* rD result: pcmpne  */

L_microblaze_selftest_reorderdata:
	DATA	0xDEADBEEF	/* rD operand        */
	DATA	0xEFBEADDE	/* rD result: swapb  */
	DATA	0xBEEFDEAD	/* rD result: swaph  */

	DATA	0x00000000	/* rD operand        */
	DATA	0x00000000	/* rD result: swapb  */
	DATA	0x00000000	/* rD result: swaph  */
/**
* @} End of "addtogroup microblaze_pseudo_asm_macro".
*/
