/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xil_mmu.h
*
* @addtogroup a53_64_mmu_apis Cortex A53 64bit Processor MMU Handling
*
* MMU function equip users to modify default memory attributes of MMU table as
* per the need.
*
* @{
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 5.00 	pkp  05/29/14 First release
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

#ifndef XIL_MMU_H
#define XIL_MMU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***************************** Include Files *********************************/

#include "xil_types.h"

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

/**
 *@cond nocomments
 */

/* Memory type */
#define NORM_NONCACHE 0x401UL 	/* Normal Non-cacheable*/
#define STRONG_ORDERED 0x409UL	/* Strongly ordered (Device-nGnRnE)*/
#define DEVICE_MEMORY 0x40DUL	/* Device memory (Device-nGnRE)*/
#define RESERVED 0x0UL			/* reserved memory*/

/* Normal write-through cacheable inner shareable*/
#define NORM_WT_CACHE 0x711UL

/* Normal write back cacheable inner-shareable */
#define NORM_WB_CACHE 0x705UL

/*
 * shareability attribute only applicable to
 * normal cacheable memory
 */
#define INNER_SHAREABLE (0x3 << 8)
#define OUTER_SHAREABLE (0x2 << 8)
#define NON_SHAREABLE	(~(0x3 << 8))

/* Execution type */
#define EXECUTE_NEVER ((0x1 << 53) | (0x1 << 54))

/* Security type */
#define NON_SECURE	(0x1 << 5)

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
/**
 *@endcond
 */

void Xil_SetTlbAttributes(UINTPTR Addr, u64 attrib);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XIL_MMU_H */
/**
* @} End of "addtogroup a53_64_mmu_apis".
*/
