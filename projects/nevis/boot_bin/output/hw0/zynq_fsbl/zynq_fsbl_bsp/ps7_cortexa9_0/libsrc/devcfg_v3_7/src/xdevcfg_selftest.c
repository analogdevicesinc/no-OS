/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xdevcfg_selftest.c
* @addtogroup devcfg_v3_7
* @{
*
* Contains diagnostic self-test functions for the XDcfg driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- ---------------------------------------------
* 1.00a hvm 02/07/11 First release
* 2.02a nm  02/27/13 Fixed CR# 701348.
*                    Peripheral test fails with  Running
* 		     DcfgSelfTestExample() in SECURE bootmode.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xdevcfg.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/****************************************************************************/
/**
*
* Run a self-test on the Device Configuration Interface. This test does a
* control register write and reads back the same value.
*
* @param	InstancePtr is a pointer to the XDcfg instance.
*
* @return
*		- XST_SUCCESS if self-test was successful.
*		- XST_FAILURE if fails.
*
* @note		None.
*
******************************************************************************/
int XDcfg_SelfTest(XDcfg *InstancePtr)
{
	u32 OldCfgReg;
	u32 CfgReg;
	int Status = XST_SUCCESS;

	/*
	 * Assert to ensure the inputs are valid and the instance has been
	 * initialized.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	OldCfgReg = XDcfg_GetControlRegister(InstancePtr);

	XDcfg_SetControlRegister(InstancePtr, XDCFG_CTRL_NIDEN_MASK);

	CfgReg = XDcfg_GetControlRegister(InstancePtr);

	if ((CfgReg & XDCFG_CTRL_NIDEN_MASK) != XDCFG_CTRL_NIDEN_MASK) {

		Status = XST_FAILURE;
	}

	/*
	 * Restore the original values of the register
	 */
	XDcfg_SetControlRegister(InstancePtr, OldCfgReg);

	return Status;
}
/** @} */
