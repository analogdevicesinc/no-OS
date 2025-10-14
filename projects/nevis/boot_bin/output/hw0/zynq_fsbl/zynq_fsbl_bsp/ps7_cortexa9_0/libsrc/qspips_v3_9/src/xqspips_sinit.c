/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xqspips_sinit.c
* @addtogroup qspips_v3_9
* @{
*
* The implementation of the XQspiPs component's static initialization
* functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- -----------------------------------------------
* 1.00  sdm 11/25/10 First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xstatus.h"
#include "xqspips.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

extern XQspiPs_Config XQspiPs_ConfigTable[];

/*****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. A table
* contains the configuration info for each device in the system.
*
* @param	DeviceId contains the ID of the device to look up the
*		configuration for.
*
* @return
*
* A pointer to the configuration found or NULL if the specified device ID was
* not found. See xqspips.h for the definition of XQspiPs_Config.
*
* @note		None.
*
******************************************************************************/
XQspiPs_Config *XQspiPs_LookupConfig(u16 DeviceId)
{
	XQspiPs_Config *CfgPtr = NULL;
	int Index;

	for (Index = 0; Index < XPAR_XQSPIPS_NUM_INSTANCES; Index++) {
		if (XQspiPs_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XQspiPs_ConfigTable[Index];
			break;
		}
	}
	return CfgPtr;
}
/** @} */
