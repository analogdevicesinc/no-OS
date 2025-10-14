/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xiicps.c
* @addtogroup Overview
* @{
*
* This section contains implementation of required functions for
* the XIicPs driver.
*
* <pre> MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- --------------------------------------------
* 1.00a drg/jz  01/30/10 First release
* 1.00a sdm     09/21/11 Updated the InstancePtr->Options in the
*			 XIicPs_CfgInitialize by calling XIicPs_GetOptions.
* 2.1   hk      04/25/14 Explicitly reset CR and clear FIFO in Abort function
*                        and state the same in the comments. CR# 784254.
*                        Fix for CR# 761060 - provision for repeated start.
* 2.3	sk		10/07/14 Repeated start feature removed.
* 3.0	sk		11/03/14 Modified TimeOut Register value to 0xFF
* 						 in XIicPs_Reset.
*				12/06/14 Implemented Repeated start feature.
*				01/31/15 Modified the code according to MISRAC 2012 Compliant.
* 3.3   kvn		05/05/16 Modified latest code for MISRA-C:2012 Compliance.
* 3.11  sd	02/06/20 Added clocking support.
* 3.11  rna	02/11/20 Moved XIicPs_Reset to xiicps_hw.c
*
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

static INLINE void StubHandler(void *CallBackRef, u32 StatusEvent);

/************************** Variable Definitions *****************************/


/*****************************************************************************/
/**
*
* @brief
* Initializes a specific XIicPs instance such that the driver is ready to use.
*
* The state of the device after initialization is:
*   - Device is disabled
*   - Slave mode
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	ConfigPtr is a reference to a structure containing information
*		about a specific IIC device. This function initializes an
*		InstancePtr object for a specific device specified by the
*		contents of Config.
* @param	EffectiveAddr is the device base address in the virtual memory
*		address space. The caller is responsible for keeping the address
*		mapping from EffectiveAddr to the device physical base address
*		unchanged once this function is invoked. Unexpected errors may
*		occur if the address mapping changes after this function is
*		called. If address translation is not used, use
*		ConfigPtr->BaseAddress for this parameter, passing the physical
*		address instead.
*
* @return	The return value is XST_SUCCESS if successful.
*
* @note		None.
*
******************************************************************************/
s32 XIicPs_CfgInitialize(XIicPs *InstancePtr, XIicPs_Config *ConfigPtr,
				  u32 EffectiveAddr)
{
	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(ConfigPtr != NULL);

	/*
	 * Set some default values.
	 */
	InstancePtr->Config.DeviceId = ConfigPtr->DeviceId;
	InstancePtr->Config.BaseAddress = EffectiveAddr;
	InstancePtr->Config.InputClockHz = ConfigPtr->InputClockHz;
#if defined  (XCLOCKING)
	InstancePtr->Config.RefClk = ConfigPtr->RefClk;
	InstancePtr->IsClkEnabled = 0;
#endif

	InstancePtr->StatusHandler = StubHandler;
	InstancePtr->CallBackRef = NULL;

	InstancePtr->IsReady = (u32)XIL_COMPONENT_IS_READY;

	/*
	 * Reset the IIC device to get it into its initial state. It is expected
	 * that device configuration will take place after this initialization
	 * is done, but before the device is started.
	 */
	XIicPs_Reset(InstancePtr);

	/*
	 * Keep a copy of what options this instance has.
	 */
	InstancePtr->Options = XIicPs_GetOptions(InstancePtr);

	/* Initialize repeated start flag to 0 */
	InstancePtr->IsRepeatedStart = 0;

	return (s32)XST_SUCCESS;
}

/*****************************************************************************/
/**
* @brief
* Check whether the I2C bus is busy
*
* @param	InstancePtr is a pointer to the XIicPs instance.
*
* @return
* 		- TRUE if the bus is busy.
*		- FALSE if the bus is not busy.
*
* @note		None.
*
******************************************************************************/
s32 XIicPs_BusIsBusy(XIicPs *InstancePtr)
{
	u32 StatusReg;
	s32 Status;

        /*
         * Assert validates the input arguments.
         */
        Xil_AssertNonvoid(InstancePtr != NULL);

	StatusReg = XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
					   XIICPS_SR_OFFSET);
	if ((StatusReg & XIICPS_SR_BA_MASK) != 0x0U) {
		Status = (s32)TRUE;
	}else {
#if defined  (XCLOCKING)
		if (InstancePtr->IsClkEnabled == 1) {
			Xil_ClockDisable(InstancePtr->Config.RefClk);
			InstancePtr->IsClkEnabled = 0;
		}
#endif
		Status = (s32)FALSE;
	}
	return Status;
}

/*****************************************************************************/
/**
*
* This is a stub for the status callback. The stub is here in case the upper
* layers forget to set the handler.
*
* @param	CallBackRef is a pointer to the upper layer callback reference.
* @param	StatusEvent is the event that just occurred.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static INLINE void StubHandler(void *CallBackRef, u32 StatusEvent)
{
        (void) ((void *)CallBackRef);
        (void) StatusEvent;
        Xil_AssertVoidAlways();
}

/*****************************************************************************/
/**
*
* @brief
* Aborts a transfer in progress by resetting the FIFOs. The byte counts are
* cleared.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void XIicPs_Abort(XIicPs *InstancePtr)
{
	u32 IntrMaskReg;
	u32 IntrStatusReg;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);

	/*
	 * Enter a critical section, so disable the interrupts while we clear
	 * the FIFO and the status register.
	 */
	IntrMaskReg = XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
					   XIICPS_IMR_OFFSET);
	XIicPs_WriteReg(InstancePtr->Config.BaseAddress,
			  XIICPS_IDR_OFFSET, XIICPS_IXR_ALL_INTR_MASK);

	/*
	 * Reset the settings in config register and clear the FIFOs.
	 */
	XIicPs_WriteReg(InstancePtr->Config.BaseAddress, XIICPS_CR_OFFSET,
			  (u32)XIICPS_CR_RESET_VALUE | (u32)XIICPS_CR_CLR_FIFO_MASK);

	/*
	 * Read, then write the interrupt status to make sure there are no
	 * pending interrupts.
	 */
	IntrStatusReg = XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
					 XIICPS_ISR_OFFSET);
	XIicPs_WriteReg(InstancePtr->Config.BaseAddress,
			  XIICPS_ISR_OFFSET, IntrStatusReg);

	/*
	 * Restore the interrupt state.
	 */
	IntrMaskReg = (u32)XIICPS_IXR_ALL_INTR_MASK & (~IntrMaskReg);
	XIicPs_WriteReg(InstancePtr->Config.BaseAddress,
			  XIICPS_IER_OFFSET, IntrMaskReg);

}

/*****************************************************************************/
/**
* Put more data into the transmit FIFO, number of bytes is ether expected
* number of bytes for this transfer or available space in FIFO, which ever
* is less.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
*
* @return	Number of bytes left for this instance.
*
* @note		This is function is shared by master and slave.
*
******************************************************************************/
s32 TransmitFifoFill(XIicPs *InstancePtr)
{
	u8 AvailBytes;
	s32 LoopCnt;
	s32 NumBytesToSend;

	/*
	 * Determine number of bytes to write to FIFO.
	 */
	AvailBytes = (u8)XIICPS_FIFO_DEPTH -
		(u8)XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
					   XIICPS_TRANS_SIZE_OFFSET);

	if (InstancePtr->SendByteCount > (s32)AvailBytes) {
		NumBytesToSend = (s32)AvailBytes;
	} else {
		NumBytesToSend = InstancePtr->SendByteCount;
	}

	/*
	 * Fill FIFO with amount determined above.
	 */
	for (LoopCnt = 0; LoopCnt < NumBytesToSend; LoopCnt++) {
		XIicPs_SendByte(InstancePtr);
	}

	return InstancePtr->SendByteCount;
}
/** @} */
