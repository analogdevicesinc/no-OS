/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_testmem.h
* @addtogroup common_test_utils Test Utilities for Memory and Caches
*
* - Cache test: xil_testcache.h contains utility functions to test cache.
*
* - I/O test: The Xil_testio.h file contains endian related memory IO functions. A
*	subset of the memory tests can be selected or all of the tests can be run in order.
*	If there is an error detected by a subtest, the test stops and the failure code is
*	returned. Further tests are not run even if all of the tests are selected.
*
* - Memory test: The xil_testmem.h file contains utility functions to test memory.
*	A subset of the memory tests can be selected or all of the tests can be run
*	in order. If there is an error detected by a subtest, the test stops and the
*	failure code is returned. Further tests are not run even if all of the tests are selected.
*
*
* Following list describes the supported memory tests:
*
*  - XIL_TESTMEM_ALLMEMTESTS: This test runs all of the subtests.
*
*  - XIL_TESTMEM_INCREMENT: This test
* starts at 'XIL_TESTMEM_INIT_VALUE' and uses the incrementing value as the
* test value for memory.
*
*  - XIL_TESTMEM_WALKONES: Also known as the Walking ones test. This test
* uses a walking '1' as the test value for memory.
* @code
*          location 1 = 0x00000001
*          location 2 = 0x00000002
*          ...
* @endcode
*
*  - XIL_TESTMEM_WALKZEROS: Also known as the Walking zero's test.
* This test uses the inverse value of the walking ones test
* as the test value for memory.
* @code
*       location 1 = 0xFFFFFFFE
*       location 2 = 0xFFFFFFFD
*       ...
*@endcode
*
*  - XIL_TESTMEM_INVERSEADDR: Also known as the inverse address test.
* This test uses the inverse of the address of the location under test
* as the test value for memory.
*
*  - XIL_TESTMEM_FIXEDPATTERN: Also known as the fixed pattern test.
* This test uses the provided patters as the test value for memory.
* If zero is provided as the pattern the test uses '0xDEADBEEF".
*
* @warning
* The tests are <b>DESTRUCTIVE</b>. Run before any initialized memory spaces
* have been set up.
* The address provided to the memory tests is not checked for
* validity except for the NULL case. It is possible to provide a code-space
* pointer for this test to start with and ultimately destroy executable code
* causing random failures.
*
* @note
* Used for spaces where the address range of the region is smaller than
* the data width. If the memory range is greater than 2 ** width,
* the patterns used in XIL_TESTMEM_WALKONES and XIL_TESTMEM_WALKZEROS will
* repeat on a boundary of a power of two making it more difficult to detect
* addressing errors. The XIL_TESTMEM_INCREMENT and XIL_TESTMEM_INVERSEADDR
* tests suffer the same problem. Ideally, if large blocks of memory are to be
* tested, break them up into smaller regions of memory to allow the test
* patterns used not to repeat over the region tested.
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
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XIL_TESTMEM_H	/* prevent circular inclusions */
#define XIL_TESTMEM_H	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xil_types.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/

/* xutil_memtest defines */

#define XIL_TESTMEM_INIT_VALUE	1U

/** @name Memory subtests
 * @{
 */
/**
 * See the detailed description of the subtests in the file description.
 */
#define XIL_TESTMEM_ALLMEMTESTS     0x00U
#define XIL_TESTMEM_INCREMENT       0x01U
#define XIL_TESTMEM_WALKONES        0x02U
#define XIL_TESTMEM_WALKZEROS       0x03U
#define XIL_TESTMEM_INVERSEADDR     0x04U
#define XIL_TESTMEM_FIXEDPATTERN    0x05U
#define XIL_TESTMEM_MAXTEST         XIL_TESTMEM_FIXEDPATTERN
/* @} */

#if !defined(__aarch64__) && !defined(__arch64__)
#define	NUM_OF_BITS_IN_BYTE	8U
#define	NUM_OF_BYTES_IN_HW	2U
#define	NUM_OF_BITS_IN_HW	16U
#define	NUM_OF_BYTES_IN_WORD	4U
#define	NUM_OF_BITS_IN_WORD	32U
#endif
/***************** Macros (Inline Functions) Definitions *********************/

/**
 *@endcond
 */

/************************** Function Prototypes ******************************/

/* xutil_testmem prototypes */
#if defined(__MICROBLAZE__) && !defined(__arch64__) && (XPAR_MICROBLAZE_ADDR_SIZE > 32)
extern s32 Xil_TestMem32(u32 AddrLow, u32 AddrHigh, u32 Words, u32 Pattern, u8 Subtest);
extern s32 Xil_TestMem16(u32 AddrLow, u32 AddrHigh, u32 Words, u16 Pattern, u8 Subtest);
extern s32 Xil_TestMem8(u32 AddrLow, u32 AddrHigh, u32 Words, u8 Pattern, u8 Subtest);
#else
extern s32 Xil_TestMem32(u32 *Addr, u32 Words, u32 Pattern, u8 Subtest);
extern s32 Xil_TestMem16(u16 *Addr, u32 Words, u16 Pattern, u8 Subtest);
extern s32 Xil_TestMem8(u8 *Addr, u32 Words, u8 Pattern, u8 Subtest);
#endif

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/**
* @} End of "addtogroup common_test_utils".
*/
