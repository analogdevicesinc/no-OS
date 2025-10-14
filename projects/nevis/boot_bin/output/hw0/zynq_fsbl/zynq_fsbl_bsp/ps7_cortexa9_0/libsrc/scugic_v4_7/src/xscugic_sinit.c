/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xscugic_sinit.c
* @addtogroup scugic_v4_7
* @{
*
* Contains static init functions for the XScuGic driver for the Interrupt
* Controller. See xscugic.h for a detailed description of the driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- --------------------------------------------------------
* 1.00a drg  01/19/10 First release
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.10  mus  07/17/18 Updated file to fix the various coding style issues
*                     reported by checkpatch. It fixes CR#1006344.
* 4.6	sk   08/05/21 Remove XScuGic_Config variable definition to fix
* 		      misrac violation.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xparameters.h"
#include "xscugic.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/


/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
*
* Looks up the device configuration based on the unique device ID. A table
* contains the configuration info for each device in the system.
*
* @param	DeviceId is the unique identifier for a device.
*
* @return	A pointer to the XScuGic configuration structure for the
*		specified device, or NULL if the device was not found.
*
* @note		None.
*
******************************************************************************/
XScuGic_Config *XScuGic_LookupConfig(u16 DeviceId)
{
	XScuGic_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < (u32)XPAR_SCUGIC_NUM_INSTANCES; Index++) {
		if (XScuGic_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XScuGic_ConfigTable[Index];
			break;
		}
	}

	return (XScuGic_Config *)CfgPtr;
}

XScuGic_Config *XScuGic_LookupConfigBaseAddr(UINTPTR BaseAddress)
{
        XScuGic_Config *CfgPtr = NULL;
        u32 Index;

        for (Index = 0U; Index < (u32)XPAR_SCUGIC_NUM_INSTANCES; Index++) {
                if (XScuGic_ConfigTable[Index].DistBaseAddress == BaseAddress) {
                        CfgPtr = &XScuGic_ConfigTable[Index];
                        break;
                }
        }
        return (XScuGic_Config *)CfgPtr;
}
/** @} */
