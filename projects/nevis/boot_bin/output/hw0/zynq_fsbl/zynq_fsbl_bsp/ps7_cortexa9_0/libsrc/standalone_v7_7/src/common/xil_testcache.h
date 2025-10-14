/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_testcache.h
*
* @addtogroup common_test_utils
* <h2>Cache test </h2>
* The xil_testcache.h file contains utility functions to test cache.
*
* @{
* <pre>
* Ver    Who    Date    Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a hbm  07/29/09 First release
* </pre>
*
******************************************************************************/

/**
*@cond nocomments
*/

#ifndef XIL_TESTCACHE_H	/* prevent circular inclusions */
#define XIL_TESTCACHE_H	/* by using protection macros */

#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern s32 Xil_TestDCacheRange(void);
extern s32 Xil_TestDCacheAll(void);
extern s32 Xil_TestICacheRange(void);
extern s32 Xil_TestICacheAll(void);

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */

/**
 *@endcond
 */

/**
* @} End of "addtogroup common_test_utils".
*/
