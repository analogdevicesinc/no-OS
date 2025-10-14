/******************************************************************************
* Copyright (c) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_cache.h
*
* @addtogroup a9_cache_apis Cortex A9 Processor Cache Functions
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
* 1.00a ecm  01/29/10 First release
* 3.04a sdm  01/02/12 Remove redundant dsb/dmb instructions in cache maintenance
*		      APIs.
* 6.8   aru  09/06/18 Removed compilation warnings for ARMCC toolchain.
* </pre>
*
******************************************************************************/

/**
*@cond nocomments
*/

#ifndef XIL_CACHE_H
#define XIL_CACHE_H

#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__

#define asm_cp15_inval_dc_line_mva_poc(param) __asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (param));

#define asm_cp15_clean_inval_dc_line_mva_poc(param) __asm__ __volatile__("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC :: "r" (param));

#define asm_cp15_inval_ic_line_mva_pou(param) __asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_IC_LINE_MVA_POU :: "r" (param));

#define asm_cp15_inval_dc_line_sw(param) __asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_DC_LINE_SW :: "r" (param));

#define asm_cp15_clean_inval_dc_line_sw(param) __asm__ __volatile__("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_SW :: "r" (param));

#elif defined (__ICCARM__)

#define asm_cp15_inval_dc_line_mva_poc(param) __asm volatile ("mcr " \
			XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (param));

#define asm_cp15_clean_inval_dc_line_mva_poc(param) __asm volatile ("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC :: "r" (param));

#define asm_cp15_inval_ic_line_mva_pou(param) __asm volatile ("mcr " \
			XREG_CP15_INVAL_IC_LINE_MVA_POU :: "r" (param));

#define asm_cp15_inval_dc_line_sw(param) __asm volatile ("mcr " \
			XREG_CP15_INVAL_DC_LINE_SW :: "r" (param));

#define asm_cp15_clean_inval_dc_line_sw(param) __asm volatile ("mcr " \
			XREG_CP15_CLEAN_INVAL_DC_LINE_SW :: "r" (param));

#endif

/**
*@endcond
*/

void Xil_DCacheEnable(void);
void Xil_DCacheDisable(void);
void Xil_DCacheInvalidate(void);
void Xil_DCacheInvalidateRange(INTPTR adr, u32 len);
void Xil_DCacheFlush(void);
void Xil_DCacheFlushRange(INTPTR adr, u32 len);

void Xil_ICacheEnable(void);
void Xil_ICacheDisable(void);
void Xil_ICacheInvalidate(void);
void Xil_ICacheInvalidateRange(INTPTR adr, u32 len);

#ifdef __cplusplus
}
#endif

#endif
/**
* @} End of "addtogroup a9_cache_apis".
*/
