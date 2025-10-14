/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_cache.h
*
* @addtogroup a53_64_cache_apis Cortex A53 64bit Processor Cache Functions
*
* Cache functions provide access to cache related operations such as flush
* and invalidate for instruction and data caches. It gives option to perform
* the cache operations on a single cacheline, a range of memory and an entire
* cache.
*
* @{
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 5.00 	pkp  05/29/14 First release
* </pre>
*
******************************************************************************/
#ifndef XIL_CACHE_H
#define XIL_CACHE_H

#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@cond nocomments
 */

/************************** Constant Definitions *****************************/
#define L1_DATA_PREFETCH_CONTROL_MASK  0xE000
#define L1_DATA_PREFETCH_CONTROL_SHIFT  13

/**
 *@endcond
 */

/***************** Macros (Inline Functions) Definitions *********************/
#define Xil_DCacheFlushRange Xil_DCacheInvalidateRange

/************************** Function Prototypes ******************************/
void Xil_DCacheEnable(void);
void Xil_DCacheDisable(void);
void Xil_DCacheInvalidate(void);
void Xil_DCacheInvalidateRange(INTPTR adr, INTPTR len);
void Xil_DCacheInvalidateLine(INTPTR adr);
void Xil_DCacheFlush(void);
void Xil_DCacheFlushLine(INTPTR adr);

void Xil_ICacheEnable(void);
void Xil_ICacheDisable(void);
void Xil_ICacheInvalidate(void);
void Xil_ICacheInvalidateRange(INTPTR adr, INTPTR len);
void Xil_ICacheInvalidateLine(INTPTR adr);
void Xil_ConfigureL1Prefetch(u8 num);
#ifdef __cplusplus
}
#endif

#endif
/**
* @} End of "addtogroup a53_64_cache_apis".
*/
