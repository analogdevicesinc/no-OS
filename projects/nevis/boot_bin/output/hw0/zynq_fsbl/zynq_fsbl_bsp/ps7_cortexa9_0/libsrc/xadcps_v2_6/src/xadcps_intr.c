/******************************************************************************
* Copyright (C) 2011 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xadcps_intr.c
* @addtogroup xadcps_v2_6
* @{
*
* This file contains interrupt handling API functions of the XADC
* device.
*
* The device must be configured at hardware build time to support interrupt
* for all the functions in this file to work.
*
* Refer to xadcps.h header file and device specification for more information.
*
* @note
*
* Calling the interrupt functions without including the interrupt component will
* result in asserts if asserts are enabled, and will result in a unpredictable
* behavior if the asserts are not enabled.
*
* <pre>
*
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- -----  -------- -----------------------------------------------------
* 1.00a ssb    12/22/11 First release based on the XPS/AXI xadc driver
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xadcps.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/


/****************************************************************************/
/**
*
* This function enables the specified interrupts in the device.
*
* @param	InstancePtr is a pointer to the XAdcPs instance.
* @param	Mask is the bit-mask of the interrupts to be enabled.
*		Bit positions of 1 will be enabled. Bit positions of 0 will
*		keep the previous setting. This mask is formed by OR'ing
*		XADCPS_INTX_* bits defined in xadcps_hw.h.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XAdcPs_IntrEnable(XAdcPs *InstancePtr, u32 Mask)
{
	u32 RegValue;

	/*
	 * Assert the arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Disable the specified interrupts in the IPIER.
	 */
	RegValue = XAdcPs_ReadReg(InstancePtr->Config.BaseAddress,
				    XADCPS_INT_MASK_OFFSET);
	RegValue &= ~(Mask & XADCPS_INTX_ALL_MASK);
	XAdcPs_WriteReg(InstancePtr->Config.BaseAddress,
				XADCPS_INT_MASK_OFFSET,
				RegValue);
}


/****************************************************************************/
/**
*
* This function disables the specified interrupts in the device.
*
* @param	InstancePtr is a pointer to the XAdcPs instance.
* @param	Mask is the bit-mask of the interrupts to be disabled.
*		Bit positions of 1 will be disabled. Bit positions of 0 will
*		keep the previous setting. This mask is formed by OR'ing
*		XADCPS_INTX_* bits defined in xadcps_hw.h.
*
* @return	None.
*
* @note		None
*
*****************************************************************************/
void XAdcPs_IntrDisable(XAdcPs *InstancePtr, u32 Mask)
{
	u32 RegValue;

	/*
	 * Assert the arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Enable the specified interrupts in the IPIER.
	 */
	RegValue = XAdcPs_ReadReg(InstancePtr->Config.BaseAddress,
				    XADCPS_INT_MASK_OFFSET);
	RegValue |= (Mask & XADCPS_INTX_ALL_MASK);
	XAdcPs_WriteReg(InstancePtr->Config.BaseAddress,
				XADCPS_INT_MASK_OFFSET,
				RegValue);
}
/****************************************************************************/
/**
*
* This function returns the enabled interrupts read from the Interrupt Mask
* Register (IPIER). Use the XADCPS_IPIXR_* constants defined in xadcps_hw.h to
* interpret the returned value.
*
* @param	InstancePtr is a pointer to the XAdcPs instance.
*
* @return	A 32-bit value representing the contents of the I.
*
* @note		None.
*
*****************************************************************************/
u32 XAdcPs_IntrGetEnabled(XAdcPs *InstancePtr)
{
	/*
	 * Assert the arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Return the value read from the Interrupt Enable Register.
	 */
	return (~ XAdcPs_ReadReg(InstancePtr->Config.BaseAddress,
				XADCPS_INT_MASK_OFFSET) & XADCPS_INTX_ALL_MASK);
}

/****************************************************************************/
/**
*
* This function returns the interrupt status read from Interrupt Status
* Register(IPISR). Use the XADCPS_IPIXR_* constants defined in xadcps_hw.h
* to interpret the returned value.
*
* @param	InstancePtr is a pointer to the XAdcPs instance.
*
* @return	A 32-bit value representing the contents of the IPISR.
*
* @note		The device must be configured at hardware build time to include
*		interrupt component for this function to work.
*
*****************************************************************************/
u32 XAdcPs_IntrGetStatus(XAdcPs *InstancePtr)
{
	/*
	 * Assert the arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Return the value read from the Interrupt Status register.
	 */
	return XAdcPs_ReadReg(InstancePtr->Config.BaseAddress,
				XADCPS_INT_STS_OFFSET) & XADCPS_INTX_ALL_MASK;
}

/****************************************************************************/
/**
*
* This function clears the specified interrupts in the Interrupt Status
* Register (IPISR).
*
* @param	InstancePtr is a pointer to the XAdcPs instance.
* @param	Mask is the bit-mask of the interrupts to be cleared.
*		Bit positions of 1 will be cleared. Bit positions of 0 will not
* 		change the previous interrupt status. This mask is formed by
* 		OR'ing XADCPS_IPIXR_* bits which are defined in xadcps_hw.h.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XAdcPs_IntrClear(XAdcPs *InstancePtr, u32 Mask)
{
	u32 RegValue;

	/*
	 * Assert the arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Clear the specified interrupts in the Interrupt Status register.
	 */
	RegValue = XAdcPs_ReadReg(InstancePtr->Config.BaseAddress,
				    XADCPS_INT_STS_OFFSET);
	RegValue &= (Mask & XADCPS_INTX_ALL_MASK);
	XAdcPs_WriteReg(InstancePtr->Config.BaseAddress, XADCPS_INT_STS_OFFSET,
			  RegValue);

}
/** @} */
