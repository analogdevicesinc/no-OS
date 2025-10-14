/******************************************************************************
* Copyright (C) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xdmaps_selftest.c
* @addtogroup dmaps_v2_8
* @{
*
* This file contains the self-test functions for the XDmaPs driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------ -------- -----------------------------------------------
* 1.00	hbm 	03/29/2010 First Release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xstatus.h"
#include "xdmaps.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Variable Definitions *****************************/


/************************** Function Prototypes ******************************/


/****************************************************************************/
/**
*
* This function runs a self-test on the driver and hardware device. This self
* test performs a local loopback and verifies data can be sent and received.
*
* The time for this test is proportional to the baud rate that has been set
* prior to calling this function.
*
* The mode and control registers are restored before return.
*
* @param	InstPtr is a pointer to the XDmaPs instance
*
* @return
*
*		- XST_SUCCESS if the test was successful
*		- XST_FAILURE if the test failed
*
* @note
*
* This function can hang if the hardware is not functioning properly.
*
******************************************************************************/
int XDmaPs_SelfTest(XDmaPs *InstPtr)
{
	u32 BaseAddr = InstPtr->Config.BaseAddress;
	int i;

	if (XDmaPs_ReadReg(BaseAddr, XDMAPS_DBGSTATUS_OFFSET)
	    & XDMAPS_DBGSTATUS_BUSY)
		return XST_FAILURE;

	for (i = 0; i < XDMAPS_CHANNELS_PER_DEV; i++) {
		if (XDmaPs_ReadReg(BaseAddr,
				    XDmaPs_CSn_OFFSET(i)))
			return XST_FAILURE;
	}
	return XST_SUCCESS;
}
/** @} */
