/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xscutimer_sinit.c
* @addtogroup scutimer_v2_4
* @{
*
* This file contains method for static initialization (compile-time) of the
* driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- ---------------------------------------------
* 1.00a nm  03/10/10 First release
* 2.1 	sk  02/26/15 Modified the code for MISRA-C:2012 compliance.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xscutimer.h"
#include "xparameters.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions ****************************/
extern XScuTimer_Config XScuTimer_ConfigTable[XPAR_XSCUTIMER_NUM_INSTANCES];

/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
* Lookup the device configuration based on the unique device ID. The table
* contains the configuration info for each device in the system.
*
* @param	DeviceId is the unique device ID of the device being looked up.
*
* @return	A pointer to the configuration table entry corresponding to the
*		given device ID, or NULL if no match is found.
*
* @note		None.
*
******************************************************************************/
XScuTimer_Config *XScuTimer_LookupConfig(u16 DeviceId)
{
	XScuTimer_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < XPAR_XSCUTIMER_NUM_INSTANCES; Index++) {
		if (XScuTimer_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XScuTimer_ConfigTable[Index];
			break;
		}
	}

	return (XScuTimer_Config *)CfgPtr;
}
/** @} */
