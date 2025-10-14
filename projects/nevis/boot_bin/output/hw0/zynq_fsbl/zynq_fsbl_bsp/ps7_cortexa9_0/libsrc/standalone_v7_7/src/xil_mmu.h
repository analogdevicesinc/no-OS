/******************************************************************************
* Copyright (c) 2012 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xil_mmu.h
*
* @addtogroup a9_mmu_apis Cortex A9 Processor MMU Functions
*
* MMU functions equip users to enable MMU, disable MMU and modify default
* memory attributes of MMU table as per the need.
*
* @{
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a sdm  01/12/12 Initial version
* 4.2	pkp	 07/21/14 Included xil_types.h file which contains definition for
*					  u32 which resolves issue of CR#805869
* 5.4	pkp	 23/11/15 Added attribute definitions for Xil_SetTlbAttributes API
* 6.8   aru  09/06/18 Removed compilation warnings for ARMCC toolchain.
* </pre>
*
*
******************************************************************************/

/**
*@cond nocomments
*/

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

/* Memory type */
#define NORM_NONCACHE 0x11DE2 	/* Normal Non-cacheable */
#define STRONG_ORDERED 0xC02	/* Strongly ordered */
#define DEVICE_MEMORY 0xC06		/* Device memory */
#define RESERVED 0x0			/* reserved memory */

/* Normal write-through cacheable shareable */
#define NORM_WT_CACHE 0x16DEA

/* Normal write back cacheable shareable */
#define NORM_WB_CACHE 0x15DE6

/* shareability attribute */
#define SHAREABLE (0x1 << 16)
#define NON_SHAREABLE	(~(0x1 << 16))

/* Execution type */
#define EXECUTE_NEVER ((0x1 << 4) | (0x1 << 0))

/**
*@endcond
*/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void Xil_SetTlbAttributes(INTPTR Addr, u32 attrib);
void Xil_EnableMMU(void);
void Xil_DisableMMU(void);
void* Xil_MemMap(UINTPTR PhysAddr, size_t size, u32 flags);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XIL_MMU_H */
/**
* @} End of "addtogroup a9_mmu_apis".
*/
