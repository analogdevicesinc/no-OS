/******************************************************************************
* Copyright (C) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xdmaps_sinit.c
* @addtogroup dmaps_v2_8
* @{
*
* The implementation of the XDmaPs driver's static initialzation
* functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00  hbm  08/13/10 First Release
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/

#include "xstatus.h"
#include "xparameters.h"
#include "xdmaps.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/


/***************** Macros (Inline Functions) Definitions ********************/


/************************** Variable Definitions ****************************/
extern XDmaPs_Config XDmaPs_ConfigTable[];

/************************** Function Prototypes *****************************/

/****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. The table
* contains the configuration info for each device in the system.
*
* @param DeviceId contains the ID of the device
*
* @return
*
* A pointer to the configuration structure or NULL if the specified device
* is not in the system.
*
* @note
*
* None.
*
******************************************************************************/
XDmaPs_Config *XDmaPs_LookupConfig(u16 DeviceId)
{
	XDmaPs_Config *CfgPtr = NULL;

	int i;

	for (i = 0; i < XPAR_XDMAPS_NUM_INSTANCES; i++) {
		if (XDmaPs_ConfigTable[i].DeviceId == DeviceId) {
			CfgPtr = &XDmaPs_ConfigTable[i];
			break;
		}
	}

	return CfgPtr;
}
/** @} */
