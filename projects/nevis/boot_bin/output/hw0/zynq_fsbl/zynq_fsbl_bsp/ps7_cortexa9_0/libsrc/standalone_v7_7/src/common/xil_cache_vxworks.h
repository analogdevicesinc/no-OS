/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_cache_vxworks.h
*
* Contains the cache related functions for VxWorks that is wrapped by
* xil_cache.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date	 Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a hbm  12/11/09 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XIL_CACHE_VXWORKS_H
#define XIL_CACHE_VXWORKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"
#include "vxLib.h"
#include "sysLibExtra.h"
#include "cacheLib.h"

#if (CPU_FAMILY==PPC)

#define Xil_DCacheEnable()		cacheEnable(DATA_CACHE)

#define Xil_DCacheDisable()		cacheDisable(DATA_CACHE)

#define Xil_DCacheInvalidateRange(Addr, Len) \
		cacheInvalidate(DATA_CACHE, (void *)(Addr), (Len))

#define Xil_DCacheFlushRange(Addr, Len) \
		cacheFlush(DATA_CACHE, (void *)(Addr), (Len))

#define Xil_ICacheEnable()		cacheEnable(INSTRUCTION_CACHE)

#define Xil_ICacheDisable()		cacheDisable(INSTRUCTION_CACHE)

#define Xil_ICacheInvalidateRange(Addr, Len) \
		cacheInvalidate(INSTRUCTION_CACHE, (void *)(Addr), (Len))


#else
#error "Unknown processor / architecture. Must be PPC for VxWorks."
#endif

#ifdef __cplusplus
}
#endif

#endif
/**
 *@endcond
 */