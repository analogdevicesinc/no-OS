/******************************************************************************
* Copyright (C) 2011 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xadcps_selftest.c
* @addtogroup xadcps_v2_6
* @{
*
* This file contains a diagnostic self test function for the XAdcPs driver.
* The self test function does a simple read/write test of the Alarm Threshold
* Register.
*
* See xadcps.h for more information.
*
* @note	None.
*
* <pre>
*
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- -----  -------- -----------------------------------------------------
* 1.00a ssb    12/22/11 First release based on the XPS/AXI xadc driver
* 2.6   aad    11/02/20 Fix MISRAC Mandatory and Advisory errors.
*
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/

#include "xadcps.h"

/************************** Constant Definitions ****************************/

/*
 * The following constant defines the test value to be written
 * to the Alarm Threshold Register
 */
#define XADCPS_ATR_TEST_VALUE 		0x55U

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

/*****************************************************************************/
/**
*
* Run a self-test on the driver/device. The test
*	- Resets the device,
*	- Writes a value into the Alarm Threshold register and reads it back
*	for comparison.
*	- Resets the device again.
*
*
* @param	InstancePtr is a pointer to the XAdcPs instance.
*
* @return
*		- XST_SUCCESS if the value read from the Alarm Threshold
*		register is the same as the value written.
*		- XST_FAILURE Otherwise
*
* @note		This is a destructive test in that resets of the device are
*		performed. Refer to the device specification for the
*		device status after the reset operation.
*
******************************************************************************/
int XAdcPs_SelfTest(XAdcPs *InstancePtr)
{
	int Status;
	u32 RegValue;

	/*
	 * Assert the argument
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);


	/*
	 * Reset the device to get it back to its default state
	 */
	XAdcPs_Reset(InstancePtr);

	/*
	 * Write a value into the Alarm Threshold registers, read it back, and
	 * do the comparison
	 */
	XAdcPs_SetAlarmThreshold(InstancePtr, XADCPS_ATR_VCCINT_UPPER,
				  XADCPS_ATR_TEST_VALUE);
	RegValue = XAdcPs_GetAlarmThreshold(InstancePtr, XADCPS_ATR_VCCINT_UPPER);

	if (RegValue == XADCPS_ATR_TEST_VALUE) {
		Status = XST_SUCCESS;
	} else {
		Status = XST_FAILURE;
	}

	/*
	 * Reset the device again to its default state.
	 */
	XAdcPs_Reset(InstancePtr);
	/*
	 * Return the test result.
	 */
	return Status;
}
/** @} */
