/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xiicps_selftest.c
* @addtogroup Overview
* @{
*
* The xiicps_selftest.c file component contains the implementation of selftest functions for the
* XIicPs driver component.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- ---------------------------------------------
* 1.00a drg/jz 01/30/10 First release
* 1.00a sdm    09/22/11 Removed unused code
* 3.0	sk	   11/03/14 Removed TimeOut Register value check
*			   01/31/15	Modified the code according to MISRAC 2012 Compliant.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xiicps.h"

/************************** Constant Definitions *****************************/

#define REG_TEST_VALUE    0x00000005U	/**< Register value to write */

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/


/************************** Variable Definitions *****************************/


/*****************************************************************************/
/**
*
* @brief
* Runs a self-test on the driver/device. The self-test is destructive in that
* a reset of the device is performed in order to check the reset values of
* the registers and to get the device into a known state.
*
* Upon successful return from the self-test, the device is reset.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
*
* @return
*		- XST_SUCCESS if successful.
*		- XST_REGISTER_ERROR indicates a register did not read or write
*		correctly
*
* @note		None.
*
******************************************************************************/
s32 XIicPs_SelfTest(XIicPs *InstancePtr)
{

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);

	/*
	 * All the IIC registers should be in their default state right now.
	 */
	if ((XIICPS_CR_RESET_VALUE !=
		 XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
				  XIICPS_CR_OFFSET)) ||
		(XIICPS_IXR_ALL_INTR_MASK !=
		 XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
				  XIICPS_IMR_OFFSET))) {
		return (s32)XST_FAILURE;
	}

	XIicPs_Reset(InstancePtr);

	/*
	 * Write, Read then write a register
	 */
	XIicPs_WriteReg(InstancePtr->Config.BaseAddress,
			  XIICPS_SLV_PAUSE_OFFSET, REG_TEST_VALUE);

	if (REG_TEST_VALUE != XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
						   XIICPS_SLV_PAUSE_OFFSET)) {
		return (s32)XST_FAILURE;
	}

	XIicPs_WriteReg(InstancePtr->Config.BaseAddress,
			  XIICPS_SLV_PAUSE_OFFSET, 0U);

	XIicPs_Reset(InstancePtr);

	return (s32)XST_SUCCESS;
}
/** @} */
