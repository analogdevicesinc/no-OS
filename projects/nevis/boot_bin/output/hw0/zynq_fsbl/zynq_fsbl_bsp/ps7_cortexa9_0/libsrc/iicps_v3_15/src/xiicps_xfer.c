/******************************************************************************
* Copyright (C) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xiicps_xfer.c
* @addtogroup Overview
* @{
*
* The xiicps_xfer.c file contains implementation of required helper functions
* for the XIicPs driver.
*
* <pre> MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- --------------------------------------------
* 3.11  rna     12/10/19 First release
*		02/18/20 Modified latest code for MISRA-C:2012 Compliance.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xiicps.h"
#include "xiicps_xfer.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/

/*****************************************************************************/
/**
* This function prepares a device to transfers as a master.
*
* @param        InstancePtr is a pointer to the XIicPs instance.
*
* @param        Role specifies whether the device is sending or receiving.
*
* @return
*               - XST_SUCCESS if everything went well.
*               - XST_FAILURE if bus is busy.
*
* @note         Interrupts are always disabled, device which needs to use
*               interrupts needs to setup interrupts after this call.
*
****************************************************************************/
s32 XIicPs_SetupMaster(XIicPs *InstancePtr, s32 Role)
{
	u32 ControlReg;
	UINTPTR BaseAddr;

	BaseAddr = InstancePtr->Config.BaseAddress;
	ControlReg = XIicPs_ReadReg(BaseAddr, XIICPS_CR_OFFSET);


	/*
	 * Only check if bus is busy when repeated start option is not set.
	 */
	if ((ControlReg & XIICPS_CR_HOLD_MASK) == 0U) {
		if (XIicPs_BusIsBusy(InstancePtr) == (s32)1) {
			return (s32)XST_FAILURE;
		}
	}

	/*
	 * Set up master, AckEn, nea and also clear fifo.
	 */
	ControlReg |= (u32)XIICPS_CR_ACKEN_MASK | (u32)XIICPS_CR_CLR_FIFO_MASK |
		(u32)XIICPS_CR_NEA_MASK | (u32)XIICPS_CR_MS_MASK;

	if (Role == RECVING_ROLE) {
		ControlReg |= (u32)XIICPS_CR_RD_WR_MASK;
	}else {
		ControlReg &= ~((u32)XIICPS_CR_RD_WR_MASK);
	}

	XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET, ControlReg);

	XIicPs_DisableAllInterrupts(BaseAddr);

	return (s32)XST_SUCCESS;
}

/*****************************************************************************/
/**
* This function handles continuation of sending data. It is invoked
* from interrupt handler.
*
* @param        InstancePtr is a pointer to the XIicPs instance.
*
* @return       None.
*
* @note         None.
*
****************************************************************************/
void MasterSendData(XIicPs *InstancePtr)
{
	(void)TransmitFifoFill(InstancePtr);

	/*
	 * Clear hold bit if done, so stop can be sent out.
	 */
	if (InstancePtr->SendByteCount == 0) {

		/*
		 * If user has enabled repeated start as an option,
		 * do not disable it.
		 */
		if (InstancePtr->IsRepeatedStart == 0) {

			XIicPs_WriteReg(InstancePtr->Config.BaseAddress,
					(u32)XIICPS_CR_OFFSET,
					XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
						(u32)XIICPS_CR_OFFSET) & ~((u32)XIICPS_CR_HOLD_MASK));
		}
	}

	return;
}

/*****************************************************************************/
/**
*
* This function handles continuation of receiving data. It is invoked
* from interrupt handler.
*
* @param        InstancePtr is a pointer to the XIicPs instance.
*
* @return       Number of bytes still expected by the instance.
*
* @note         None.
*
****************************************************************************/
s32 SlaveRecvData(XIicPs *InstancePtr)
{
	u32 StatusReg;
	UINTPTR BaseAddr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	BaseAddr = InstancePtr->Config.BaseAddress;

	StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);

	while (((StatusReg & XIICPS_SR_RXDV_MASK)!=0x0U) &&
			(InstancePtr->RecvByteCount > 0)) {
		XIicPs_RecvByte(InstancePtr);
		StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);
	}

	return InstancePtr->RecvByteCount;
}
/** @} */
