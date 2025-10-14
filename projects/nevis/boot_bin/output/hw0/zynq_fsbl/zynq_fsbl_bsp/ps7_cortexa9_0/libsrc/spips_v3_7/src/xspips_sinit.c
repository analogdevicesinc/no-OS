/******************************************************************************
* Copyright (C) 2010 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xspips_sinit.c
* @addtogroup spips_v3_7
* @{
*
* The implementation of the XSpiPs driver's static initialization
* functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- -----------------------------------------------
* 1.00  drg/jz 01/25/10 First release
* 3.00  kvn    02/13/15 Modified code for MISRA-C:2012 compliance.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xstatus.h"
#include "xspips.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
extern XSpiPs_Config XSpiPs_ConfigTable[XPAR_XSPIPS_NUM_INSTANCES];

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
* not found. See xspips.h for the definition of XSpiPs_Config.
*
* @note		None.
*
******************************************************************************/
XSpiPs_Config *XSpiPs_LookupConfig(u16 DeviceId)
{
	XSpiPs_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < (u32)XPAR_XSPIPS_NUM_INSTANCES; Index++) {
		if (XSpiPs_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XSpiPs_ConfigTable[Index];
			break;
		}
	}
	return (XSpiPs_Config *)CfgPtr;
}
/** @} */
