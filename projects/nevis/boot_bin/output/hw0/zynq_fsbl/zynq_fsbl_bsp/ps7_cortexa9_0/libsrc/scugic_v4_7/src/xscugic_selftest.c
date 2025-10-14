/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xscugic_selftest.c
* @addtogroup scugic_v4_7
* @{
*
* Contains diagnostic self-test functions for the XScuGic driver.
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a drg  01/19/10 First release
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.10  mus  07/17/18 Updated file to fix the various coding style issues
*                     reported by checkpatch. It fixes CR#1006344.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xscugic.h"

/************************** Constant Definitions *****************************/

#define	XSCUGIC_PCELL_ID	0xB105F00DU

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
*
* Run a self-test on the driver/device. This test reads the ID registers and
* compares them.
*
* @param	InstancePtr is a pointer to the XScuGic instance.
*
* @return
*
*		-XST_SUCCESS if self-test is successful.
*		-XST_FAILURE if the self-test is not successful.
*
* @note		None.
*
******************************************************************************/
s32  XScuGic_SelfTest(XScuGic *InstancePtr)
{
	u32 RegValue1 = 0U;
	u32 Index;
	s32 Status;

	/*
	 * Assert the arguments
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Read the ID registers.
	 */
	for (Index = 0U; Index <= 3U; Index++) {
		RegValue1 |= XScuGic_DistReadReg(InstancePtr,
			((u32)XSCUGIC_PCELLID_OFFSET + (Index * 4U))) <<
			(Index * 8U);
	}

	if (XSCUGIC_PCELL_ID != RegValue1) {
		Status = XST_FAILURE;
	} else {
		Status = XST_SUCCESS;
	}
	return Status;
}
/** @} */
