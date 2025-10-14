/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file nand.h
*
* This file contains the interface for the NAND FLASH functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	01/10/10 Initial release
* 2.00a mb	30/05/12 added the flag XPAR_PS7_NAND_0_BASEADDR
* 10.00a kc 08/04/14 Fix for CR#809336 - Removed smc.h
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___NAND_H___
#define ___NAND_H___


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/


#ifdef XPAR_PS7_NAND_0_BASEADDR

#include "xnandps.h"
#include "xnandps_bbm.h"
/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
u32 InitNand(void);

u32 NandAccess( u32 SourceAddress,
                u32 DestinationAddress,
                u32 LengthWords);
#endif
/************************** Variable Definitions *****************************/


#ifdef __cplusplus
}
#endif


#endif /* ___NAND_H___ */

