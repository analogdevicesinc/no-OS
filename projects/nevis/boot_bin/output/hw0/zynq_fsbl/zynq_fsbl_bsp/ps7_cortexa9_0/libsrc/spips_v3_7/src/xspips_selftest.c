/******************************************************************************
* Copyright (C) 2010 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xspips_selftest.c
* @addtogroup spips_v3_7
* @{
*
* This component contains the implementation of selftest functions for an SPI
* device.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- ----------------------------------------------
* 1.00  drg/jz 01/25/10 First release
* 1.04a	sg     01/30/13 SetDelays test includes DelayTestNss parameter.
* 3.00  kvn    02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.2   aru    01/20/19 Fixes violations according to MISRAC-2012
*                       in safety mode and done changes such as
*                       Added goto statements.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xspips.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/


/*****************************************************************************/
/**
*
* Runs a self-test on the driver/device. The self-test is destructive in that
* a reset of the device is performed in order to check the reset values of
* the registers and to get the device into a known state.
*
* Upon successful return from the self-test, the device is reset.
*
* @param	InstancePtr is a pointer to the XSpiPs instance.
*
* @return
* 		- XST_SUCCESS if successful
*		- XST_REGISTER_ERROR indicates a register did not read or write
*		correctly.
*
* @note		None.
*
******************************************************************************/
s32 XSpiPs_SelfTest(XSpiPs *InstancePtr)
{
	s32 Status;
	u32 Register;
	u8 DelayTestNss;
	u8 DelayTestBtwn;
	u8 DelayTestAfter;
	u8 DelayTestInit;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Reset the SPI device to leave it in a known good state
	 */
	XSpiPs_Reset(InstancePtr);

	/*
	 * All the SPI registers should be in their default state right now.
	 */
	Register = XSpiPs_ReadReg(InstancePtr->Config.BaseAddress,
				 XSPIPS_CR_OFFSET);
	if (Register != XSPIPS_CR_RESET_STATE) {
		Status = (s32)XST_REGISTER_ERROR;
		goto END;
	}

	Register = XSpiPs_ReadReg(InstancePtr->Config.BaseAddress,
				 XSPIPS_SR_OFFSET);
	if (Register != XSPIPS_ISR_RESET_STATE) {
		Status = (s32)XST_REGISTER_ERROR;
		goto END;;
	}

	DelayTestNss = 0x5AU;
	DelayTestBtwn = 0xA5U;
	DelayTestAfter = 0xAAU;
	DelayTestInit = 0x55U;

	/*
	 * Write and read the delay register, just to be sure there is some
	 * hardware out there.
	 */
	Status = XSpiPs_SetDelays(InstancePtr, DelayTestNss, DelayTestBtwn,
				   DelayTestAfter, DelayTestInit);
	if (Status != (s32)XST_SUCCESS) {
		goto END;
	}

	XSpiPs_GetDelays(InstancePtr, &DelayTestNss, &DelayTestBtwn,
			&DelayTestAfter, &DelayTestInit);
	if ((0x5AU != DelayTestNss) || (0xA5U != DelayTestBtwn) ||
		(0xAAU != DelayTestAfter) || (0x55U != DelayTestInit)) {
		Status = (s32)XST_REGISTER_ERROR;
		goto END;
	}

	Status = XSpiPs_SetDelays(InstancePtr, 0U, 0U, 0U, 0U);
	if (Status != (s32)XST_SUCCESS) {
		goto END;
	}

	/*
	 * Reset the SPI device to leave it in a known good state
	 */
	XSpiPs_Reset(InstancePtr);

	Status = (s32)XST_SUCCESS;

	END:
	return Status;
}
/** @} */
