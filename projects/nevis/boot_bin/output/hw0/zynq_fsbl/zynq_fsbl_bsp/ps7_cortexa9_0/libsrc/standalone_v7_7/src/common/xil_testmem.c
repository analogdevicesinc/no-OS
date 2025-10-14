/******************************************************************************
* Copyright (c) 2009 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_testmem.c
* @addtogroup common_test_utils
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver    Who    Date    Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a hbm  08/25/09 First release
* 7.5   mus  03/10/21 Added new set of Xil_TestMem32, Xil_TestMem16 and
*                     Xil_TestMem8 APIs to support memory test for memory
*                     regions mapped at extended addresses
*                     (addresses > 4 GB). These new set of APIs would be
*                     compiled only for 32 bit Microblaze processor, if
*                     XPAR_MICROBLAZE_ADDR_SIZE is greater than 32.
*                     It fixes CR#1089129.
* 7.6   mus  07/29/21 Updated Xil_TestMem8 to fix issues reported by static
*                     analysis tool. It fixes CR#1105956.
* 7.7	sk   01/10/22 Remove commented macro to fix misra_c_2012_directive_4_4
* 		      violation.
* 7.7	sk   01/10/22 Modify operands to fix misra_c_2012_rule_10_1 violation.
* 7.7	sk   01/10/22 Typecast to make the both left and right sides expressions
* 		      of same type and fix misra_c_2012_rule_10_6 violation.
* 7.7	sk   01/10/22 Modify varaiable name from I to i to fix misra_c_2012_rule_
* 		      21_2 violation.
* 7.7	sk   01/10/22 Remove arithematic operations on pointer varaible to fix
* 		      misra_c_2012_rule_18_4 violation.
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/
#include "xil_testmem.h"
#include "xil_io.h"
#include "xil_assert.h"

/************************** Constant Definitions ****************************/
/************************** Function Prototypes *****************************/

static u32 RotateLeft(u32 Input, u8 Width);

/* define ROTATE_RIGHT to give access to this functionality */
#ifdef ROTATE_RIGHT
static u32 RotateRight(u32 Input, u8 Width);
#endif /* ROTATE_RIGHT */

#if defined(__MICROBLAZE__) && !defined(__arch64__) && (XPAR_MICROBLAZE_ADDR_SIZE > 32)

/*****************************************************************************/
/**
*
* @brief    Perform a destructive 8-bit wide memory test.
*
* @param    Addrlow: lower 32 bit address of memory to be tested.
* @param    Addrhigh: upper 32 bit address of memory to be tested.
* @param    Words: length of the block.
* @param    Pattern: constant used for the constant pattern test, if 0,
*           0xDEADBEEF is used.
* @param    Subtest: type of test selected. See xil_testmem.h for possible
*	        values.
*
* @return
*           - -1 is returned for a failure
*           - 0 is returned for a pass
*
* @note
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** Width,
* the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* repeat on a boundary of a power of two making it more difficult to detect
* addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
*
*****************************************************************************/
s32 Xil_TestMem8(u32 Addrlow, u32 Addrhigh, u32 Words, u8 Pattern, u8 Subtest)
{
	u32 I;
	u32 j;
	u8 Val;
	u8 WordMem8;
	s32 Status = 0;
	u64  Addr = (Addrlow + ((u64)Addrhigh << 32));

	Xil_AssertNonvoid(Words != (u32)0);
	Xil_AssertNonvoid(Subtest <= XIL_TESTMEM_MAXTEST);

	/*
	 * variable initialization
	 */
	Val = XIL_TESTMEM_INIT_VALUE;

	/*
	 * select the proper Subtest(s)
	 */

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INCREMENT)) {
		/*
		 * Fill the memory with incrementing
		 * values starting from XIL_TESTMEM_INIT_VALUE
		 */
		for (I = 0U; I < Words; I++) {
			/* write memory location */
			sbea(Addr+I, Val);
			Val++;
		}
		/*
		 * Restore the reference 'Val' to the
		 * initial value
		 */
		Val = XIL_TESTMEM_INIT_VALUE;
		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the incrementing reference
		 * Val
		 */

		for (I = 0U; I < Words; I++) {
			/* read memory location */
			WordMem8 = lbuea(Addr+I);
			if (WordMem8 != Val) {
				Status = -1;
				goto End_Label;
			}
			Val++;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKONES)) {
		/*
		 * set up to cycle through all possible initial
		 * test Patterns for walking ones test
		 */

		for (j = 0U; j < NUM_OF_BITS_IN_BYTE; j++) {
			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */
			Val = (u8)((u32)1 << j);
			/*
			 * START walking ones test
			 * Write a one to each data bit indifferent locations
			 */
			for (I = 0U; I < NUM_OF_BITS_IN_BYTE; I++) {
				/* write memory location */
				sbea(Addr+I, Val);
				Val = (u8)RotateLeft(Val, 8U);
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = (u8)((u32)1 << j);
			/* Read the values from each location that was written */
			for (I = 0U; I < NUM_OF_BITS_IN_BYTE; I++) {
				/* read memory location */
				WordMem8 = lbuea(Addr+I);
				if (WordMem8 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = (u8)RotateLeft(Val, NUM_OF_BITS_IN_BYTE);
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKZEROS)) {
		/*
		 * set up to cycle through all possible initial test
		 * Patterns for walking zeros test
		 */

		for (j = 0U; j < NUM_OF_BITS_IN_BYTE; j++) {
			/*
			 * Generate an initial value for walking ones test to test
			 * for bad data bits
			 */
			Val = (u8) (~(1U << j));
			/*
			 * START walking zeros test
			 * Write a one to each data bit indifferent locations
			 */
			for (I = 0U; I < NUM_OF_BITS_IN_BYTE; I++) {
				/* write memory location */
				sbea(Addr+I, Val);
				Val = ~((u8)RotateLeft(~Val, NUM_OF_BITS_IN_BYTE));
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = (u8) (~(1U << j));
			/* Read the values from each location that was written */
			for (I = 0U; I < NUM_OF_BITS_IN_BYTE; I++) {
				/* read memory location */
				WordMem8 = lbuea(Addr+I);
				if (WordMem8 != Val) {
					Status = -1;
					goto End_Label;
				}

				Val = ~((u8)RotateLeft(~Val, NUM_OF_BITS_IN_BYTE));
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INVERSEADDR)) {
		/* Fill the memory with inverse of address */
		for (I = 0U; I < Words; I++) {
			/* write memory location */
			Val = (u8) (~((INTPTR) (Addr + I)));
			sbea(Addr+I, Val);
		}

		/*
		 * Check every word within the words
		 * of tested memory
		 */

		for (I = 0U; I < Words; I++) {
			/* read memory location */
			WordMem8 = lbuea(Addr+I);
			Val = (u8) (~((INTPTR) (Addr+I)));
			if ((WordMem8 ^ Val) != 0x00U) {
				Status = -1;
				goto End_Label;
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_FIXEDPATTERN)) {
		/*
		 * Generate an initial value for
		 * memory testing
		 */

		if (Pattern == (u8)0) {
			Val = 0xA5U;
		}
		else {
			Val = Pattern;
		}
		/*
		 * Fill the memory with fixed Pattern
		 */
		for (I = 0U; I < Words; I++) {
			/* write memory location */
			sbea(Addr+I, Val);
		}
		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the fixed Pattern
		 */

		for (I = 0U; I < Words; I++) {
			/* read memory location */
			WordMem8 = lbuea(Addr+I);
			if (WordMem8 != Val) {
				Status = -1;
				goto End_Label;
			}
		}
	}

End_Label:
	return Status;
}

/*****************************************************************************/
/**
*
* @brief    Perform a destructive 16-bit wide memory test.
*
* @param    Addrlow: lower 32 bit address of memory to be tested.
* @param    Addrhigh: upper 32 bit address of memory to be tested.
* @param    Words: length of the block.
* @param    Pattern: constant used for the constant Pattern test, if 0,
*           0xDEADBEEF is used.
* @param    Subtest: type of test selected. See xil_testmem.h for possible
*	        values.
*
* @return
*
*           - -1 is returned for a failure
*           - 0 is returned for a pass
*
* @note
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** Width,
* the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* repeat on a boundary of a power of two making it more difficult to detect
* addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
*
*****************************************************************************/
s32 Xil_TestMem16(u32 Addrlow,u32 Addrhigh, u32 Words, u16 Pattern, u8 Subtest)
{
	u32 I;
	u32 j;
	u16 Val;
	u16 WordMem16;
	s32 Status = 0;
	u64  Addr = (Addrlow + ((u64)Addrhigh << 32));

	Xil_AssertNonvoid(Words != (u32)0);
	Xil_AssertNonvoid(Subtest <= XIL_TESTMEM_MAXTEST);

	/*
	 * variable initialization
	 */
	Val = XIL_TESTMEM_INIT_VALUE;

	/*
	 * selectthe proper Subtest(s)
	 */

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INCREMENT)) {
		/*
		 * Fill the memory with incrementing
		 * values starting from 'XIL_TESTMEM_INIT_VALUE'
		 */
		for (I = 0U; I < (NUM_OF_BYTES_IN_HW * Words);) {
			/* write memory location */
			shea(Addr+I, Val);
			Val++;
			I = I + NUM_OF_BYTES_IN_HW;
		}

		/*
		 * Restore the reference 'Val' to the
		 * initial value
		 */
		Val = XIL_TESTMEM_INIT_VALUE;

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the incrementing reference val
		 */

		for (I = 0U; I < (NUM_OF_BYTES_IN_HW * Words);) {
			/* read memory location */
			WordMem16 = lhuea(Addr+I);
			if (WordMem16 != Val) {
				Status = -1;
				goto End_Label;
			}
			Val++;
			I = I + NUM_OF_BYTES_IN_HW;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKONES)) {
		/*
		 * set up to cycle through all possible initial test
		 * Patterns for walking ones test
		 */

		for (j = 0U; j < NUM_OF_BITS_IN_HW; j++) {
			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */

			Val = (u16)((u32)1 << j);
			/*
			 * START walking ones test
			 * Write a one to each data bit indifferent locations
			 */

			for (I = 0U; I < (NUM_OF_BYTES_IN_HW * NUM_OF_BITS_IN_HW); ) {
				/* write memory location */
				shea(Addr+I,Val);
				Val = (u16)RotateLeft(Val, 16U);
				I = I + NUM_OF_BYTES_IN_HW;
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = (u16)((u32)1 << j);
			/* Read the values from each location that was written */
			for (I = 0U; I < (NUM_OF_BYTES_IN_HW * NUM_OF_BITS_IN_HW); ) {
				/* read memory location */
				WordMem16 = lhuea(Addr+I);
				if (WordMem16 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = (u16)RotateLeft(Val, NUM_OF_BITS_IN_HW);
				I = I + NUM_OF_BYTES_IN_HW;
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKZEROS)) {
		/*
		 * set up to cycle through all possible initial
		 * test Patterns for walking zeros test
		 */

		for (j = 0U; j < NUM_OF_BITS_IN_HW; j++) {
			/*
			 * Generate an initial value for walking ones
			 * test to test for bad
			 * data bits
			 */

			Val = ~(1U << j);
			/*
			 * START walking zeros test
			 * Write a one to each data bit indifferent locations
			 */

			for (I = 0U; I < (NUM_OF_BYTES_IN_HW * NUM_OF_BITS_IN_HW);) {
				shea(Addr+I, Val);
				Val = ~((u16)RotateLeft(~Val, 16U));
				I = I + NUM_OF_BYTES_IN_HW;
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = ~(1U << j);
			/* Read the values from each location that was written */
			for (I = 0U; I < (NUM_OF_BYTES_IN_HW * NUM_OF_BITS_IN_HW); ) {
				WordMem16= lhuea(Addr+I);
				if (WordMem16 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = ~((u16)RotateLeft(~Val, NUM_OF_BITS_IN_HW));
				I = I + NUM_OF_BYTES_IN_HW;
			}

		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INVERSEADDR)) {
		/* Fill the memory with inverse of address */
		for (I = 0U; I < (NUM_OF_BYTES_IN_HW * Words);) {
			/* write memory location */
			Val = (u16) (~((INTPTR)((Addr+I))));
			shea(Addr+I, Val);
			I = I + NUM_OF_BYTES_IN_HW;
		}
		/*
		 * Check every word within the words
		 * of tested memory
		 */

		for (I = 0U; I < (NUM_OF_BYTES_IN_HW*Words); ) {
			/* read memory location */
			WordMem16 = lhuea(Addr+I);
			Val = (u16) (~((INTPTR) ((Addr+I))));
			if ((WordMem16 ^ Val) != 0x0000U) {
				Status = -1;
				goto End_Label;
			}
			I = I + NUM_OF_BYTES_IN_HW;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_FIXEDPATTERN)) {
		/*
		 * Generate an initial value for
		 * memory testing
		 */
		if (Pattern == (u16)0) {
			Val = 0xDEADU;
		}
		else {
			Val = Pattern;
		}

		/*
		 * Fill the memory with fixed pattern
		 */

		for (I = 0U; I < (2*Words);) {
			/* write memory location */
			shea(Addr+I, Val);
			I = I + NUM_OF_BYTES_IN_HW;
		}

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the fixed pattern
		 */

		for (I = 0U; I < (NUM_OF_BYTES_IN_HW * Words);) {
			/* read memory location */
			WordMem16=lhuea(Addr+I);
			if (WordMem16 != Val) {
				Status = -1;
				goto End_Label;
			}
			I = I + NUM_OF_BYTES_IN_HW;
		}
	}
End_Label:
	return Status;
}

/*****************************************************************************/
/**
*
* @brief    Perform a destructive 32-bit wide memory test.
*
* @param    Addrlow: lower 32 bit address of memory to be tested.
* @param    Addrhigh: upper 32 bit address of memory to be tested.
* @param    Words: length of the block.
* @param    Pattern: constant used for the constant pattern test, if 0,
*           0xDEADBEEF is used.
* @param    Subtest: test type selected. See xil_testmem.h for possible
*	        values.
*
* @return
*           - 0 is returned for a pass
*           - 1 is returned for a failure
*
* @note
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** Width,
* the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* repeat on a boundary of a power of two making it more difficult to detect
* addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
*
*****************************************************************************/
s32 Xil_TestMem32(u32 Addrlow, u32 Addrhigh, u32 Words, u32 Pattern, u8 Subtest)
{
	u32 I;
	u32 j;
	u32 Val;
	u32 WordMem32;
	s32 Status = 0;
	u64  Addr = (Addrlow + ((u64)Addrhigh << 32));

	Xil_AssertNonvoid(Words != (u32)0);
	Xil_AssertNonvoid(Subtest <= (u8)XIL_TESTMEM_MAXTEST);

	/*
	 * variable initialization
	 */
	Val = XIL_TESTMEM_INIT_VALUE;


	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INCREMENT)) {
		/*
		 * Fill the memory with incrementing
		 * values starting from 'XIL_TESTMEM_INIT_VALUE'
		 */
		for (I = 0U; I <(NUM_OF_BYTES_IN_WORD * Words);) {
			swea(Addr+I, Val);
			Val++;
			I = I + NUM_OF_BYTES_IN_WORD;
		}

		/*
		 * Restore the reference 'Val' to the
		 * initial value
		 */
		Val = XIL_TESTMEM_INIT_VALUE;

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the incrementing reference
		 * Val
		 */

		for (I = 0U; I < ( NUM_OF_BYTES_IN_WORD * Words);) {
			WordMem32 = lwea(Addr+I);

			if (WordMem32 != Val) {
				Status = -1;
				goto End_Label;
			}

			Val++;
			I = I + NUM_OF_BYTES_IN_WORD;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKONES)) {
		/*
		 * set up to cycle through all possible initial
		 * test Patterns for walking ones test
		 */

		for (j = 0U; j < NUM_OF_BITS_IN_WORD; j++) {
			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */

			Val = (1U << j);

			/*
			 * START walking ones test
			 * Write a one to each data bit indifferent locations
			 */

			for (I = 0U; I < (NUM_OF_BYTES_IN_WORD * NUM_OF_BITS_IN_WORD);) {
				/* write memory location */
				swea(Addr+I, Val);
				Val = (u32) RotateLeft(Val, NUM_OF_BITS_IN_WORD);
				I = I + NUM_OF_BYTES_IN_WORD;
			}

			/*
			 * Restore the reference 'val' to the
			 * initial value
			 */
			Val = 1U << j;

			/* Read the values from each location that was
			 * written */
			for (I = 0U; I < ((u32)32 * NUM_OF_BYTES_IN_WORD);) {
				/* read memory location */

				WordMem32 = lwea(Addr+I);

				if (WordMem32 != Val) {
					Status = -1;
					goto End_Label;
				}

				Val = (u32)RotateLeft(Val, NUM_OF_BITS_IN_WORD);
				I = I + NUM_OF_BYTES_IN_WORD;
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKZEROS)) {
		/*
		 * set up to cycle through all possible
		 * initial test Patterns for walking zeros test
		 */

		for (j = 0U; j < NUM_OF_BITS_IN_WORD; j++) {

			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */

			Val = ~(1U << j);

			/*
			 * START walking zeros test
			 * Write a one to each data bit indifferent locations
			 */

			for (I = 0U; I < (NUM_OF_BITS_IN_WORD * NUM_OF_BYTES_IN_WORD);) {
				/* write memory location */
				swea(Addr+I, Val);
				Val = ~((u32)RotateLeft(~Val, NUM_OF_BITS_IN_WORD));
				I = I + NUM_OF_BYTES_IN_WORD;
			}

			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */

			Val = ~(1U << j);

			/* Read the values from each location that was
			 * written */
			for (I = 0U; I < (NUM_OF_BITS_IN_WORD * NUM_OF_BYTES_IN_WORD);) {
				/* read memory location */
				WordMem32 = lwea(Addr+I);
				if (WordMem32 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = ~((u32)RotateLeft(~Val, NUM_OF_BITS_IN_WORD));
				I = I + NUM_OF_BYTES_IN_WORD;
			}

		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INVERSEADDR)) {
		/* Fill the memory with inverse of address */
		for (I = 0U; I < (NUM_OF_BYTES_IN_WORD * Words);) {
			/* write memory location */
			Val = (u32) (~((INTPTR) (Addr+I)));
			swea(Addr+I, Val);
			I = I + NUM_OF_BYTES_IN_WORD;
		}

		/*
		 * Check every word within the words
		 * of tested memory
		 */

		for (I = 0U; I < (NUM_OF_BYTES_IN_WORD * Words);) {
			/* Read the location */
			WordMem32 = lwea(Addr+I);
			Val = (u32) (~((INTPTR) (Addr+I)));

			if ((WordMem32 ^ Val) != 0x00000000U) {
				Status = -1;
				goto End_Label;
			}
			I = I + NUM_OF_BYTES_IN_WORD;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_FIXEDPATTERN)) {
		/*
		 * Generate an initial value for
		 * memory testing
		 */

		if (Pattern == (u32)0) {
			Val = 0xDEADBEEFU;
		}
		else {
			Val = Pattern;
		}

		/*
		 * Fill the memory with fixed Pattern
		 */

		for (I = 0U; I < (NUM_OF_BYTES_IN_WORD * Words);) {
			/* write memory location */
			swea(Addr+I, Val);
			I = I + NUM_OF_BYTES_IN_WORD;
		}

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the fixed Pattern
		 */

		for (I = 0U; I < (NUM_OF_BYTES_IN_WORD * Words);) {

			/* read memory location */

			WordMem32 = lwea(Addr+I);
			if (WordMem32 != Val) {
				Status = -1;
				goto End_Label;
			}
			I = I + NUM_OF_BYTES_IN_WORD;
		}
	}

End_Label:
	return Status;
}

#else
/*****************************************************************************/
/**
*
* @brief    Perform a destructive 32-bit wide memory test.
*
* @param    Addr: pointer to the region of memory to be tested.
* @param    Words: length of the block.
* @param    Pattern: constant used for the constant pattern test, if 0,
*           0xDEADBEEF is used.
* @param    Subtest: test type selected. See xil_testmem.h for possible
*	        values.
*
* @return
*           - 0 is returned for a pass
*           - 1 is returned for a failure
*
* @note
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** Width,
* the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* repeat on a boundary of a power of two making it more difficult to detect
* addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
*
*****************************************************************************/
s32 Xil_TestMem32(u32 *Addr, u32 Words, u32 Pattern, u8 Subtest)
{
	u32 i;
	u32 j;
	u32 Val;
	u32 FirtVal;
	u32 WordMem32;
	s32 Status = 0;

	Xil_AssertNonvoid(Words != (u32)0);
	Xil_AssertNonvoid(Subtest <= (u8)XIL_TESTMEM_MAXTEST);

	/*
	 * variable initialization
	 */
	Val = XIL_TESTMEM_INIT_VALUE;
	FirtVal = XIL_TESTMEM_INIT_VALUE;


	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INCREMENT)) {
		/*
		 * Fill the memory with incrementing
		 * values starting from 'FirtVal'
		 */
		for (i = 0U; i < Words; i++) {
			Addr[i] = Val;
			Val++;
		}

		/*
		 * Restore the reference 'Val' to the
		 * initial value
		 */
		Val = FirtVal;

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the incrementing reference
		 * Val
		 */

		for (i = 0U; i < Words; i++) {
			WordMem32 = Addr[i];

			if (WordMem32 != Val) {
				Status = -1;
				goto End_Label;
			}

			Val++;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKONES)) {
		/*
		 * set up to cycle through all possible initial
		 * test Patterns for walking ones test
		 */

		for (j = 0U; j < (u32)32; j++) {
			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */

			Val = (1UL << j);

			/*
			 * START walking ones test
			 * Write a one to each data bit indifferent locations
			 */

			for (i = 0U; i < (u32)32; i++) {
				/* write memory location */
				Addr[i] = Val;
				Val = (u32) RotateLeft(Val, 32U);
			}

			/*
			 * Restore the reference 'val' to the
			 * initial value
			 */
			Val = 1UL << j;

			/* Read the values from each location that was
			 * written */
			for (i = 0U; i < (u32)32; i++) {
				/* read memory location */

				WordMem32 = Addr[i];

				if (WordMem32 != Val) {
					Status = -1;
					goto End_Label;
				}

				Val = (u32)RotateLeft(Val, 32U);
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKZEROS)) {
		/*
		 * set up to cycle through all possible
		 * initial test Patterns for walking zeros test
		 */

		for (j = 0U; j < (u32)32; j++) {

			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */

			Val = ~(1UL << j);

			/*
			 * START walking zeros test
			 * Write a one to each data bit indifferent locations
			 */

			for (i = 0U; i < (u32)32; i++) {
				/* write memory location */
				Addr[i] = Val;
				Val = ~((u32)RotateLeft(~Val, 32U));
			}

			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */

			Val = ~(1UL << j);

			/* Read the values from each location that was
			 * written */
			for (i = 0U; i < (u32)32; i++) {
				/* read memory location */
				WordMem32 = Addr[i];
				if (WordMem32 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = ~((u32)RotateLeft(~Val, 32U));
			}

		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INVERSEADDR)) {
		/* Fill the memory with inverse of address */
		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Val = ~(u32) (UINTPTR) &Addr[i];
			Addr[i] = Val;
		}

		/*
		 * Check every word within the words
		 * of tested memory
		 */

		for (i = 0U; i < Words; i++) {
			/* Read the location */
			WordMem32 = Addr[i];
			Val = ~(u32) (UINTPTR) &Addr[i];

			if ((WordMem32 ^ Val) != 0x00000000U) {
				Status = -1;
				goto End_Label;
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_FIXEDPATTERN)) {
		/*
		 * Generate an initial value for
		 * memory testing
		 */

		if (Pattern == (u32)0) {
			Val = 0xDEADBEEFU;
		}
		else {
			Val = Pattern;
		}

		/*
		 * Fill the memory with fixed Pattern
		 */

		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Addr[i] = Val;
		}

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the fixed Pattern
		 */

		for (i = 0U; i < Words; i++) {

			/* read memory location */

			WordMem32 = Addr[i];
			if (WordMem32 != Val) {
				Status = -1;
				goto End_Label;
			}
		}
	}

End_Label:
	return Status;
}

/*****************************************************************************/
/**
*
* @brief    Perform a destructive 16-bit wide memory test.
*
* @param    Addr: pointer to the region of memory to be tested.
* @param    Words: length of the block.
* @param    Pattern: constant used for the constant Pattern test, if 0,
*           0xDEADBEEF is used.
* @param    Subtest: type of test selected. See xil_testmem.h for possible
*	        values.
*
* @return
*
*           - -1 is returned for a failure
*           - 0 is returned for a pass
*
* @note		Used for spaces where the address range of the region is smaller than
* 			the data width. If the memory range is greater than 2 ** Width,
*			the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* 			repeat on a boundary of a power of two making it more difficult to detect
* 			addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* 			tests suffer the same problem. Ideally, if large blocks of memory are to be
* 			tested, break them up into smaller regions of memory to allow the test
* 			patterns used not to repeat over the region tested.
*
*****************************************************************************/
s32 Xil_TestMem16(u16 *Addr, u32 Words, u16 Pattern, u8 Subtest)
{
	u32 i;
	u32 j;
	u16 Val;
	u16 FirtVal;
	u16 WordMem16;
	s32 Status = 0;

	Xil_AssertNonvoid(Words != (u32)0);
	Xil_AssertNonvoid(Subtest <= XIL_TESTMEM_MAXTEST);

	/*
	 * variable initialization
	 */
	Val = XIL_TESTMEM_INIT_VALUE;
	FirtVal = XIL_TESTMEM_INIT_VALUE;

	/*
	 * selectthe proper Subtest(s)
	 */

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INCREMENT)) {
		/*
		 * Fill the memory with incrementing
		 * values starting from 'FirtVal'
		 */
		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Addr[i] = Val;
			Val++;
		}
		/*
		 * Restore the reference 'Val' to the
		 * initial value
		 */
		Val = FirtVal;

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the incrementing reference val
		 */

		for (i = 0U; i < Words; i++) {
			/* read memory location */
			WordMem16 = Addr[i];
			if (WordMem16 != Val) {
				Status = -1;
				goto End_Label;
			}
			Val++;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKONES)) {
		/*
		 * set up to cycle through all possible initial test
		 * Patterns for walking ones test
		 */

		for (j = 0U; j < (u32)16; j++) {
			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */

			Val = (u16)((u32)1 << j);
			/*
			 * START walking ones test
			 * Write a one to each data bit indifferent locations
			 */

			for (i = 0U; i < (u32)16; i++) {
				/* write memory location */
				Addr[i] = Val;
				Val = (u16)RotateLeft(Val, 16U);
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = (u16)((u32)1 << j);
			/* Read the values from each location that was written */
			for (i = 0U; i < (u32)16; i++) {
				/* read memory location */
				WordMem16 = Addr[i];
				if (WordMem16 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = (u16)RotateLeft(Val, 16U);
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKZEROS)) {
		/*
		 * set up to cycle through all possible initial
		 * test Patterns for walking zeros test
		 */

		for (j = 0U; j < (u32)16; j++) {
			/*
			 * Generate an initial value for walking ones
			 * test to test for bad
			 * data bits
			 */

			Val = (u16) (~((u16)1U << j));
			/*
			 * START walking zeros test
			 * Write a one to each data bit indifferent locations
			 */

			for (i = 0U; i < (u32)16; i++) {
				/* write memory location */
				Addr[i] = Val;
				Val = ~((u16)RotateLeft(~((u32)Val), 16U));
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = (u16) (~((u16)1U << j));
			/* Read the values from each location that was written */
			for (i = 0U; i < (u32)16; i++) {
				/* read memory location */
				WordMem16 = Addr[i];
				if (WordMem16 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = ~((u16)RotateLeft(~((u32)Val), 16U));
			}

		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INVERSEADDR)) {
		/* Fill the memory with inverse of address */
		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Val = ~(u16) (UINTPTR) &Addr[i];
			Addr[i] = Val;
		}
		/*
		 * Check every word within the words
		 * of tested memory
		 */

		for (i = 0U; i < Words; i++) {
			/* read memory location */
			WordMem16 = Addr[i];
			Val = ~(u16) (UINTPTR) &Addr[i];
			if ((WordMem16 ^ Val) != 0x0000U) {
				Status = -1;
				goto End_Label;
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_FIXEDPATTERN)) {
		/*
		 * Generate an initial value for
		 * memory testing
		 */
		if (Pattern == (u16)0) {
			Val = 0xDEADU;
		}
		else {
			Val = Pattern;
		}

		/*
		 * Fill the memory with fixed pattern
		 */

		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Addr[i] = Val;
		}

		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the fixed pattern
		 */

		for (i = 0U; i < Words; i++) {
			/* read memory location */
			WordMem16 = Addr[i];
			if (WordMem16 != Val) {
				Status = -1;
				goto End_Label;
			}
		}
	}

End_Label:
	return Status;
}


/*****************************************************************************/
/**
*
* @brief    Perform a destructive 8-bit wide memory test.
*
* @param    Addr: pointer to the region of memory to be tested.
* @param    Words: length of the block.
* @param    Pattern: constant used for the constant pattern test, if 0,
*           0xDEADBEEF is used.
* @param    Subtest: type of test selected. See xil_testmem.h for possible
*	        values.
*
* @return
*           - -1 is returned for a failure
*           - 0 is returned for a pass
*
* @note
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** Width,
* the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* repeat on a boundary of a power of two making it more difficult to detect
* addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
*
*****************************************************************************/
s32 Xil_TestMem8(u8 *Addr, u32 Words, u8 Pattern, u8 Subtest)
{
	u32 i;
	u32 j;
	u8 Val;
	u8 FirtVal;
	u8 WordMem8;
	s32 Status = 0;

	Xil_AssertNonvoid(Words != (u32)0);
	Xil_AssertNonvoid(Subtest <= XIL_TESTMEM_MAXTEST);

	/*
	 * variable initialization
	 */
	Val = XIL_TESTMEM_INIT_VALUE;
	FirtVal = XIL_TESTMEM_INIT_VALUE;

	/*
	 * select the proper Subtest(s)
	 */

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INCREMENT)) {
		/*
		 * Fill the memory with incrementing
		 * values starting from 'FirtVal'
		 */
		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Addr[i] = Val;
			Val++;
		}
		/*
		 * Restore the reference 'Val' to the
		 * initial value
		 */
		Val = FirtVal;
		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the incrementing reference
		 * Val
		 */

		for (i = 0U; i < Words; i++) {
			/* read memory location */
			WordMem8 = Addr[i];
			if (WordMem8 != Val) {
				Status = -1;
				goto End_Label;
			}
			Val++;
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKONES)) {
		/*
		 * set up to cycle through all possible initial
		 * test Patterns for walking ones test
		 */

		for (j = 0U; j < (u32)8; j++) {
			/*
			 * Generate an initial value for walking ones test
			 * to test for bad data bits
			 */
			Val = (u8)((u32)1 << j);
			/*
			 * START walking ones test
			 * Write a one to each data bit indifferent locations
			 */
			for (i = 0U; i < (u32)8; i++) {
				/* write memory location */
				Addr[i] = Val;
				Val = (u8)RotateLeft(Val, 8U);
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = (u8)((u32)1 << j);
			/* Read the values from each location that was written */
			for (i = 0U; i < (u32)8; i++) {
				/* read memory location */
				WordMem8 = Addr[i];
				if (WordMem8 != Val) {
					Status = -1;
					goto End_Label;
				}
				Val = (u8)RotateLeft(Val, 8U);
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_WALKZEROS)) {
		/*
		 * set up to cycle through all possible initial test
		 * Patterns for walking zeros test
		 */

		for (j = 0U; j < (u32)8; j++) {
			/*
			 * Generate an initial value for walking ones test to test
			 * for bad data bits
			 */
			Val = (u8) (~(1U << j));
			/*
			 * START walking zeros test
			 * Write a one to each data bit indifferent locations
			 */
			for (i = 0U; i < (u32)8; i++) {
				/* write memory location */
				Addr[i] = Val;
				Val = ~((u8)RotateLeft(~((u32)Val), 8U));
			}
			/*
			 * Restore the reference 'Val' to the
			 * initial value
			 */
			Val = (u8) (~(1U << j));
			/* Read the values from each location that was written */
			for (i = 0U; i < (u32)8; i++) {
				/* read memory location */
				WordMem8 = Addr[i];
				if (WordMem8 != Val) {
					Status = -1;
					goto End_Label;
				}

				Val = ~((u8)RotateLeft(~((u32)Val), 8U));
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_INVERSEADDR)) {
		/* Fill the memory with inverse of address */
		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Val = ~(u8) (UINTPTR) &Addr[i];
			Addr[i] = Val;
		}

		/*
		 * Check every word within the words
		 * of tested memory
		 */

		for (i = 0U; i < Words; i++) {
			/* read memory location */
			WordMem8 = Addr[i];
			Val = ~(u8) (UINTPTR) &Addr[i];
			if ((WordMem8 ^ Val) != 0x00U) {
				Status = -1;
				goto End_Label;
			}
		}
	}

	if((Subtest == XIL_TESTMEM_ALLMEMTESTS) || (Subtest == XIL_TESTMEM_FIXEDPATTERN)) {
		/*
		 * Generate an initial value for
		 * memory testing
		 */

		if (Pattern == (u8)0) {
			Val = 0xA5U;
		}
		else {
			Val = Pattern;
		}
		/*
		 * Fill the memory with fixed Pattern
		 */
		for (i = 0U; i < Words; i++) {
			/* write memory location */
			Addr[i] = Val;
		}
		/*
		 * Check every word within the words
		 * of tested memory and compare it
		 * with the fixed Pattern
		 */

		for (i = 0U; i < Words; i++) {
			/* read memory location */
			WordMem8 = Addr[i];
			if (WordMem8 != Val) {
				Status = -1;
				goto End_Label;
			}
		}
	}

End_Label:
	return Status;
}
#endif

/*****************************************************************************/
/**
*
* @brief   Rotates the provided value to the left one bit position
*
* @param    Input is value to be rotated to the left
* @param    Width is the number of bits in the input data
*
* @return   The resulting unsigned long value of the rotate left
*
*
*****************************************************************************/
static u32 RotateLeft(u32 Input, u8 Width)
{
	u32 Msb;
	u32 ReturnVal;
	u32 WidthMask;
	u32 MsbMask;
	u32 LocalInput = Input;

	/*
	 * set up the WidthMask and the MsbMask
	 */

	MsbMask = 1UL << (Width - 1U);

	WidthMask = (MsbMask << (u32)1) - (u32)1;

	/*
	 * set the Width of the Input to the correct width
	 */

	LocalInput = LocalInput & WidthMask;

	Msb = LocalInput & MsbMask;

	ReturnVal = LocalInput << 1U;

	if (Msb != 0x00000000U) {
		ReturnVal = ReturnVal | (u32)0x00000001;
	}

	ReturnVal = ReturnVal & WidthMask;

	return ReturnVal;

}

#ifdef ROTATE_RIGHT
/*****************************************************************************/
/**
*
* @brief    Rotates the provided value to the right one bit position
*
* @param    Input: value to be rotated to the right
* @param    Width: number of bits in the input data
*
* @return
*           The resulting u32 value of the rotate right
*
*****************************************************************************/
static u32 RotateRight(u32 Input, u8 Width)
{
	u32 Lsb;
	u32 ReturnVal;
	u32 WidthMask;
	u32 MsbMask;
	u32 LocalInput = Input;
	/*
	 * set up the WidthMask and the MsbMask
	 */

	MsbMask = 1U << (Width - 1U);

	WidthMask = (MsbMask << 1U) - 1U;

	/*
	 * set the width of the input to the correct width
	 */

	LocalInput = LocalInput & WidthMask;

	ReturnVal = LocalInput >> 1U;

	Lsb = LocalInput & 0x00000001U;

	if (Lsb != 0x00000000U) {
		ReturnVal = ReturnVal | MsbMask;
	}

	ReturnVal = ReturnVal & WidthMask;

	return ReturnVal;

}
#endif /* ROTATE_RIGHT */
