/******************************************************************************
* Copyright (c) 2015 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_cache.h
*
* @addtogroup a53_32_cache_apis Cortex A53 32bit Processor Cache Functions
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
* 5.2	pkp  28/05/15 First release
* </pre>
*
******************************************************************************/
#ifndef XIL_CACHE_H
#define XIL_CACHE_H

#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void Xil_DCacheEnable(void);
void Xil_DCacheDisable(void);
void Xil_DCacheInvalidate(void);
void Xil_DCacheInvalidateRange(INTPTR adr, u32 len);
void Xil_DCacheFlush(void);
void Xil_DCacheFlushRange(INTPTR adr, u32 len);
void Xil_DCacheInvalidateLine(u32 adr);
void Xil_DCacheFlushLine(u32 adr);

void Xil_ICacheInvalidateLine(u32 adr);
void Xil_ICacheEnable(void);
void Xil_ICacheDisable(void);
void Xil_ICacheInvalidate(void);
void Xil_ICacheInvalidateRange(INTPTR adr, u32 len);

#ifdef __cplusplus
}
#endif

#endif
/**
* @} End of "addtogroup a53_64_cache_apis".
*/
