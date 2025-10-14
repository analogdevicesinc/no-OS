/******************************************************************************
* Copyright (C) 2002 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xiic_stats.c
* @addtogroup Overview
* @{
*
* Contains statistics functions for the XIic component.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- --- ------- -----------------------------------------------
* 1.01b jhl 3/26/02 repartioned the driver
* 1.01c ecm 12/05/02 new rev
* 1.13a wgr 03/22/07 Converted to new coding style.
* 2.00a ktn 10/22/09 Converted all register accesses to 32 bit access.
*		     Updated to use the HAL APIs/macros.
*		     XIic_ClearStats function is updated as the
*		     macro XIIC_CLEAR_STATS has been removed.
* </pre>
*
****************************************************************************/

/***************************** Include Files *******************************/

#include "xiic.h"
#include "xiic_i.h"

/************************** Constant Definitions ***************************/

/**************************** Type Definitions *****************************/

/***************** Macros (Inline Functions) Definitions *******************/

/************************** Function Prototypes ****************************/

/************************** Variable Definitions **************************/

/*****************************************************************************/
/**
*
* Gets a copy of the statistics for an IIC device.
*
* @param	InstancePtr is a pointer to the XIic instance to be worked on.
* @param	StatsPtr is a pointer to a XIicStats structure which will get a
*		copy of current statistics.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void XIic_GetStats(XIic *InstancePtr, XIicStats * StatsPtr)
{
	u8 NumBytes;
	u8 *SrcPtr;
	u8 *DestPtr;

	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(StatsPtr != NULL);

	/*
	 * Setup pointers to copy the stats structure
	 */
	SrcPtr = (u8 *) &InstancePtr->Stats;
	DestPtr = (u8 *) StatsPtr;

	/*
	 * Copy the current statistics to the structure passed in
	 */
	for (NumBytes = 0; NumBytes < sizeof(XIicStats); NumBytes++) {
		*DestPtr++ = *SrcPtr++;
	}
}

/*****************************************************************************/
/**
*
* Clears the statistics for the IIC device by zeroing all counts.
*
* @param	InstancePtr is a pointer to the XIic instance to be worked on.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void XIic_ClearStats(XIic *InstancePtr)
{
	u8 NumBytes;
	u8 *DestPtr;

	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(InstancePtr != NULL);

	DestPtr = (u8 *)&InstancePtr->Stats;
	for (NumBytes = 0; NumBytes < sizeof(XIicStats); NumBytes++) {
		*DestPtr++ = 0;
	}

}
/** @} */
