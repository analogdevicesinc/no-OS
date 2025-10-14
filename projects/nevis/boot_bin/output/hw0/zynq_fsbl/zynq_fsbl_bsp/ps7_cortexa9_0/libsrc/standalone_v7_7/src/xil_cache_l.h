/******************************************************************************
* Copyright (c) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_cache_l.h
*
* Contains L1 and L2 specific functions for the ARM cache functionality
* used by xcache.c. This functionality is being made available here for
* more sophisticated users.
*
* @addtogroup a9_cache_apis
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a ecm  01/24/10 First release
* 6.8   aru  09/06/18 Removed compilation warnings for ARMCC toolchain.
* </pre>
*
******************************************************************************/
#ifndef XIL_CACHE_MACH_H
#define XIL_CACHE_MACH_H

#include "xil_types.h"
#ifdef __cplusplus
extern "C" {
#endif

/************************** Function Prototypes ******************************/

void Xil_DCacheInvalidateLine(u32 adr);
void Xil_DCacheFlushLine(u32 adr);
void Xil_DCacheStoreLine(u32 adr);
void Xil_ICacheInvalidateLine(u32 adr);

void Xil_L1DCacheEnable(void);
void Xil_L1DCacheDisable(void);
void Xil_L1DCacheInvalidate(void);
void Xil_L1DCacheInvalidateLine(u32 adr);
void Xil_L1DCacheInvalidateRange(u32 adr, u32 len);
void Xil_L1DCacheFlush(void);
void Xil_L1DCacheFlushLine(u32 adr);
void Xil_L1DCacheFlushRange(u32 adr, u32 len);
void Xil_L1DCacheStoreLine(u32 adr);

void Xil_L1ICacheEnable(void);
void Xil_L1ICacheDisable(void);
void Xil_L1ICacheInvalidate(void);
void Xil_L1ICacheInvalidateLine(u32 adr);
void Xil_L1ICacheInvalidateRange(u32 adr, u32 len);

void Xil_L2CacheEnable(void);
void Xil_L2CacheDisable(void);
void Xil_L2CacheInvalidate(void);
void Xil_L2CacheInvalidateLine(u32 adr);
void Xil_L2CacheInvalidateRange(u32 adr, u32 len);
void Xil_L2CacheFlush(void);
void Xil_L2CacheFlushLine(u32 adr);
void Xil_L2CacheFlushRange(u32 adr, u32 len);
void Xil_L2CacheStoreLine(u32 adr);

#ifdef __cplusplus
}
#endif

#endif
/**
* @} End of "addtogroup a9_cache_apis".
*/
