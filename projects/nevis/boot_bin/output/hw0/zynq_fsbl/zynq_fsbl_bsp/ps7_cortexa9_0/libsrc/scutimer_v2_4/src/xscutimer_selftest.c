/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xscutimer_selftest.c
* @addtogroup scutimer_v2_4
* @{
*
* Contains diagnostic self-test functions for the XScuTimer driver.
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

/************************** Constant Definitions *****************************/

#define XSCUTIMER_SELFTEST_VALUE	0xA55AF00FU

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/****************************************************************************/
/**
*
* Run a self-test on the timer. This test clears the timer enable bit in
* the control register, writes to the timer load register and verifies the
* value read back matches the value written and restores the control register
* and the timer load register.
*
* @param	InstancePtr is a pointer to the XScuTimer instance.
*
* @return
*		- XST_SUCCESS if self-test was successful.
*		- XST_FAILURE if self test was not successful.
*
* @note		None.
*
******************************************************************************/
s32 XScuTimer_SelfTest(XScuTimer *InstancePtr)
{
	u32 Register;
	u32 CtrlOrig;
	u32 LoadOrig;
	s32 Status;

	/*
	 * Assert to ensure the inputs are valid and the instance has been
	 * initialized.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Save the contents of the Control Register and stop the timer.
	 */
	CtrlOrig = XScuTimer_ReadReg(InstancePtr->Config.BaseAddr,
				  XSCUTIMER_CONTROL_OFFSET);
	Register = CtrlOrig & (u32)(~XSCUTIMER_CONTROL_ENABLE_MASK);
	XScuTimer_WriteReg(InstancePtr->Config.BaseAddr,
			XSCUTIMER_CONTROL_OFFSET, Register);

	/*
	 * Save the contents of the Load Register.
	 * Load a new test value in the Load Register, read it back and
	 * compare it with the written value.
	 */
	LoadOrig = XScuTimer_ReadReg((InstancePtr)->Config.BaseAddr,
				  XSCUTIMER_LOAD_OFFSET);
	XScuTimer_LoadTimer(InstancePtr, XSCUTIMER_SELFTEST_VALUE);
	Register = XScuTimer_ReadReg((InstancePtr)->Config.BaseAddr,
				  XSCUTIMER_LOAD_OFFSET);

	/*
	 * Restore the contents of the Load Register and Control Register.
	 */
	XScuTimer_LoadTimer(InstancePtr, LoadOrig);
	XScuTimer_WriteReg(InstancePtr->Config.BaseAddr,
			XSCUTIMER_CONTROL_OFFSET, CtrlOrig);

	/*
	 * Return a Failure if the contents of the Load Register do not
	 * match with the value written to it.
	 */
	if (Register != XSCUTIMER_SELFTEST_VALUE) {
		Status = (s32)XST_FAILURE;
	}
	else {
		Status = (s32)XST_SUCCESS;
	}

	return Status;
}
/** @} */
