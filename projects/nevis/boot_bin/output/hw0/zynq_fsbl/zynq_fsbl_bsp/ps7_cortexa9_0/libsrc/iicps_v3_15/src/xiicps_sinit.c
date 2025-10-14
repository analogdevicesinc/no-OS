/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xiicps_sinit.c
* @addtogroup Overview
* @{
*
* The xiicps_sinit.c file contains implementation of the XIicPs component's
* static initialization functionality.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- --------------------------------------------
* 1.00a drg/jz 01/30/10 First release
* 3.00	sk	   01/31/15	Modified the code according to MISRAC 2012 Compliant.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xstatus.h"
#include "xparameters.h"
#include "xiicps.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
*
* @brief
* Looks up the device configuration based on the unique device ID. A table
* contains the configuration info for each device in the system.
*
* @param	DeviceId contains the ID of the device to look up the
*		configuration for.
*
* @return	A pointer to the configuration found or NULL if the specified
*		device ID was not found. See xiicps.h for the definition of
*		XIicPs_Config.
*
* @note		None.
*
******************************************************************************/
XIicPs_Config *XIicPs_LookupConfig(u16 DeviceId)
{
	XIicPs_Config *CfgPtr = NULL;
	s32 Index;

	for (Index = 0; Index < XPAR_XIICPS_NUM_INSTANCES; Index++) {
		if (XIicPs_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XIicPs_ConfigTable[Index];
			break;
		}
	}

	return (XIicPs_Config *)CfgPtr;
}
/** @} */
