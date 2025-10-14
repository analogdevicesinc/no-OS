/******************************************************************************
* Copyright (c) 2015 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xil_mmu.h
*
* @addtogroup a53_32_mmu_apis Cortex A53 32bit Processor MMU Handling
* @{
* MMU functions equip users to enable MMU, disable MMU and modify default
* memory attributes of MMU table as per the need.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 5.2	pkp  28/05/15 First release
* 7.1	mus  30/07/19 Added constant definitions for memory attributes.
* </pre>
*
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

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void Xil_SetTlbAttributes(UINTPTR Addr, u32 attrib);
void Xil_EnableMMU(void);
void Xil_DisableMMU(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XIL_MMU_H */
/**
* @} End of "addtogroup a53_32_mmu_apis".
*/
