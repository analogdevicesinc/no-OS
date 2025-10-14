/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file nor.h
*
* This file contains the interface for the NOR FLASH functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm	01/10/10 Initial release
* 10.00a kc 08/04/14 Fix for CR#809336 - Removed smc.h
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___NOR_H___
#define ___NOR_H___


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

#define XPS_NOR_BASEADDR 	XPS_PARPORT0_BASEADDR

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/


void InitNor(void);

u32 NorAccess( u32 SourceAddress,
	       u32 DestinationAddress,
	       u32 LengthBytes);

/************************** Variable Definitions *****************************/
#ifdef __cplusplus
}
#endif


#endif /* ___NOR_H___ */

