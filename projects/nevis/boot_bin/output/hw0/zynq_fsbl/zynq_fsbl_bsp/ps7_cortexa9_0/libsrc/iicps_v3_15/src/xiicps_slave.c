/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xiicps_slave.c
* @addtogroup Overview
* @{
*
* The xiicps_slave.c file handles slave transfers.
*
* <pre> MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --  -------- ---------------------------------------------
* 1.00a jz  01/30/10 First release
* 1.04a kpc 08/30/13 Avoid buffer overwrite in SlaveRecvData function
* 3.00	sk	01/31/15 Modified the code according to MISRAC 2012 Compliant.
* 3.3   kvn 05/05/16 Modified latest code for MISRA-C:2012 Compliance.
* 3.8   ask 08/01/18 Fix for Cppcheck and Doxygen warnings.
* 3.10 sg   06/24/19 Fix for Slave send polled and interruput transfers.
* 3.11  rna 12/20/19 Clear the ISR before enabling interrupts in Send/Receive.
*           12/23/19 Add 10 bit address support for Master/Slave
* 3.11  sd  02/06/20 Added clocking support.
* 3.11  rna 02/12/20 Moved static data transfer functions to xiicps_xfer.c file
*	    02/18/20 Modified latest code for MISRA-C:2012 Compliance.
*       rna 04/09/20 Added timeout as event in slave interrupt handler.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xiicps.h"
#include "sleep.h"
#include "xiicps_xfer.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************* Variable Definitions *****************************/

/*****************************************************************************/
/**
* @brief
* This function sets up the device to be a slave.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	SlaveAddr is the address of the slave we are receiving from.
*
* @return	None.
*
* @note
*	Interrupt is always enabled no matter the transfer is interrupt-
*	driven or polled mode. Whether device will be interrupted or not
*	depends on whether the device is connected to an interrupt
*	controller and interrupt for the device is enabled.
*
****************************************************************************/
void XIicPs_SetupSlave(XIicPs *InstancePtr, u16 SlaveAddr)
{
	u32 ControlReg;
	UINTPTR BaseAddr;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);
	Xil_AssertVoid((u16)XIICPS_ADDR_MASK >= SlaveAddr);

#if defined  (XCLOCKING)
	if (InstancePtr->IsClkEnabled == 0) {
		Xil_ClockEnable(InstancePtr->Config.RefClk);
		InstancePtr->IsClkEnabled = 1;
	}
#endif

	BaseAddr = InstancePtr->Config.BaseAddress;

	ControlReg = XIicPs_In32(BaseAddr + XIICPS_CR_OFFSET);

	/*
	 * Set up master, AckEn and also clear fifo.
	 */
	ControlReg |= (u32)XIICPS_CR_ACKEN_MASK | (u32)XIICPS_CR_CLR_FIFO_MASK;
	ControlReg &= ~((u32)XIICPS_CR_MS_MASK);

	/*
	 * Check if 10 bit address option is set. Clear/Set NEA accordingly.
	 */
	if (InstancePtr->Is10BitAddr == 1) {
		ControlReg &= ~((u32)XIICPS_CR_NEA_MASK);
	} else {
		ControlReg |= (u32)(XIICPS_CR_NEA_MASK);
	}

	XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
			  ControlReg);

	XIicPs_DisableAllInterrupts(BaseAddr);

	XIicPs_WriteReg(InstancePtr->Config.BaseAddress,
			  XIICPS_ADDR_OFFSET, (u32)SlaveAddr);

	return;
}

/*****************************************************************************/
/**
* @brief
* This function setup a slave interrupt-driven send. It set the repeated
* start for the device is the transfer size is larger than FIFO depth.
* Data processing for the send is initiated by the interrupt handler.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the send buffer.
* @param	ByteCount is the number of bytes to be sent.
*
* @return	None.
*
* @note		This send routine is for interrupt-driven transfer only.
*
****************************************************************************/
void XIicPs_SlaveSend(XIicPs *InstancePtr, u8 *MsgPtr, s32 ByteCount)
{
	UINTPTR BaseAddr;

	/*
	 * Assert validates the input arguments
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(MsgPtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);


	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->SendBufferPtr = MsgPtr;
	InstancePtr->SendByteCount = ByteCount;
	InstancePtr->RecvBufferPtr = NULL;

	/*
	 * Clear the interrupt status register.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, XIICPS_IXR_ALL_INTR_MASK);

	XIicPs_EnableInterrupts(BaseAddr,
			(u32)XIICPS_IXR_DATA_MASK | (u32)XIICPS_IXR_COMP_MASK |
			(u32)XIICPS_IXR_TO_MASK | (u32)XIICPS_IXR_NACK_MASK |
			(u32)XIICPS_IXR_TX_OVR_MASK);
}

/*****************************************************************************/
/**
* @brief
* This function setup a slave interrupt-driven receive.
* Data processing for the receive is handled by the interrupt handler.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the receive buffer.
* @param	ByteCount is the number of bytes to be received.
*
* @return	None.
*
* @note		This routine is for interrupt-driven transfer only.
*
****************************************************************************/
void XIicPs_SlaveRecv(XIicPs *InstancePtr, u8 *MsgPtr, s32 ByteCount)
{
	UINTPTR BaseAddr;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(MsgPtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);


	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->RecvBufferPtr = MsgPtr;
	InstancePtr->RecvByteCount = ByteCount;
	InstancePtr->SendBufferPtr = NULL;

	/*
	 * Clear the interrupt status register.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, XIICPS_IXR_ALL_INTR_MASK);

	XIicPs_EnableInterrupts(BaseAddr,
		(u32)XIICPS_IXR_DATA_MASK | (u32)XIICPS_IXR_COMP_MASK |
		(u32)XIICPS_IXR_NACK_MASK | (u32)XIICPS_IXR_TO_MASK |
		(u32)XIICPS_IXR_RX_OVR_MASK | (u32)XIICPS_IXR_RX_UNF_MASK);

}

/*****************************************************************************/
/**
* @brief
* This function sends  a buffer in polled mode as a slave.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the send buffer.
* @param	ByteCount is the number of bytes to be sent.
*
* @return
*		- XST_SUCCESS if everything went well.
*		- XST_FAILURE if master sends us data or master terminates the
*		transfer before all data has sent out.
*
* @note		This send routine is for polled mode transfer only.
*
****************************************************************************/
s32 XIicPs_SlaveSendPolled(XIicPs *InstancePtr, u8 *MsgPtr, s32 ByteCount)
{
	u32 IntrStatusReg;
	u32 StatusReg;
	UINTPTR BaseAddr;
	s32 Tmp;
	s32 BytesToSend;
	s32 Error = 0;
	s32 Status = (s32)XST_SUCCESS;
	_Bool Value;
	_Bool Result;
	volatile u32 RegValue;
	u32 Timeout = XIICPS_POLL_DEFAULT_TIMEOUT_VAL;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(MsgPtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);

	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->SendBufferPtr = MsgPtr;
	InstancePtr->SendByteCount = ByteCount;

	/*
	 * Use RXRW bit in status register to wait master to start a read.
	 */
	StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);
	Result = (((u32)(StatusReg & XIICPS_SR_RXRW_MASK) == (u32)0x0U));

	while (Result != FALSE) {

		/*
		 * If master tries to send us data, it is an error.
		 */
		if ((StatusReg & XIICPS_SR_RXDV_MASK) != 0x0U) {
			Error = 1;
		}

		StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);
		Result = (((u32)(StatusReg & XIICPS_SR_RXRW_MASK) == (u32)0x0U) &&
				(Error == 0));
	}

	if (Error != 0) {
		Status = (s32)XST_FAILURE;
	} else {

		/*
		 * Clear the interrupt status register.
		 */
		IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
		XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, IntrStatusReg);

		/*
		 * Send data as long as there is more data to send and
		 * there are no errors.
		 */
		Value = (InstancePtr->SendByteCount > (s32)0) &&
						((Error == 0));
		while (Value != FALSE) {

			/*
			 * Find out how many can be sent.
			 */
			BytesToSend = InstancePtr->SendByteCount;
			if (BytesToSend > (s32)(XIICPS_FIFO_DEPTH)) {
				BytesToSend = (s32)(XIICPS_FIFO_DEPTH);
			}

			for(Tmp = 0; Tmp < BytesToSend; Tmp ++) {
				XIicPs_SendByte(InstancePtr);
			}

			StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);

			/*
			 * Wait for master to read the data out of fifo.
			 */
			while (((StatusReg & XIICPS_SR_TXDV_MASK) != (u32)0x00U) &&
							(Error == 0)) {

				/*
				 * If master terminates the transfer before all data is
				 * sent, it is an error.
				 */
				IntrStatusReg = XIicPs_ReadReg(BaseAddr,
				XIICPS_ISR_OFFSET);
				if ((IntrStatusReg & XIICPS_IXR_NACK_MASK) != 0x0U) {
					Error = 1;
				}

				/* Clear ISR.
				 */
				XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET,
							IntrStatusReg);

				StatusReg = XIicPs_ReadReg(BaseAddr,
						XIICPS_SR_OFFSET);
			}
			Value = ((InstancePtr->SendByteCount > (s32)0) &&
							(Error == 0));
		}

		/*
		* Wait for transfer completion and clear the status
		*/
		while(Timeout != 0U) {
			RegValue = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
			if((RegValue & XIICPS_IXR_COMP_MASK) == XIICPS_IXR_COMP_MASK) {
				break;
			}
			usleep(1000U);
			Timeout--;
		}

		if (Timeout == 0U) {
			Status = (s32)XST_FAILURE;
		}

		XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, RegValue);


	}
	if (Error != 0) {
		Status = (s32)XST_FAILURE;
	}

	return Status;
}
/*****************************************************************************/
/**
* @brief
* This function receives a buffer in polled mode as a slave.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the receive buffer.
* @param	ByteCount is the number of bytes to be received.
*
* @return
*		- XST_SUCCESS if everything went well.
*		- XST_FAILURE if timed out.
*
* @note		This receive routine is for polled mode transfer only.
*
****************************************************************************/
s32 XIicPs_SlaveRecvPolled(XIicPs *InstancePtr, u8 *MsgPtr, s32 ByteCount)
{
	u32 IntrStatusReg;
	u32 StatusReg;
	UINTPTR BaseAddr;
	s32 Count;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(MsgPtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);

	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->RecvBufferPtr = MsgPtr;
	InstancePtr->RecvByteCount = ByteCount;

	/*
	 * Clear the interrupt status register.
	 */
	IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, IntrStatusReg);

	/*
	 * Clear the status register.
	 */
	StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);
	XIicPs_WriteReg(BaseAddr, XIICPS_SR_OFFSET, StatusReg);

	StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);
	Count = InstancePtr->RecvByteCount;
	while (Count > (s32)0) {

		/* Wait for master to put data */
		while ((StatusReg & XIICPS_SR_RXDV_MASK) == 0U) {
		    StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);

			/*
			 * If master terminates the transfer before we get all
			 * the data or the master tries to read from us,
			 * it is an error.
			 */
			IntrStatusReg = XIicPs_ReadReg(BaseAddr,
						XIICPS_ISR_OFFSET);
			if (((IntrStatusReg & (XIICPS_IXR_DATA_MASK |
					XIICPS_IXR_COMP_MASK))!=0x0U) &&
				((StatusReg & XIICPS_SR_RXDV_MASK) == 0U) &&
				(InstancePtr->RecvByteCount > 0)) {

				return (s32)XST_FAILURE;
			}

			/*
			 * Clear the interrupt status register.
			 */
			XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET,
			IntrStatusReg);
		}

		/*
		 * Read all data from FIFO.
		 */
		while (((StatusReg & XIICPS_SR_RXDV_MASK)!=0x0U) &&
			 (InstancePtr->RecvByteCount > 0)){

			XIicPs_RecvByte(InstancePtr);

			StatusReg = XIicPs_ReadReg(BaseAddr,
				XIICPS_SR_OFFSET);
		}
		Count = InstancePtr->RecvByteCount;
	}

	return (s32)XST_SUCCESS;
}

/*****************************************************************************/
/**
* The interrupt handler for slave mode. It does the protocol handling for
* the interrupt-driven transfers.
*
* Completion events and errors are signaled to upper layer for proper
* handling.
*
* The interrupts that are handled are:
* - DATA
*	If the instance is sending, it means that the master wants to read more
*	data from us. Send more data, and check whether we are done with this
*	send.
*
*	If the instance is receiving, it means that the master has written
* 	more data to us. Receive more data, and check whether we are done with
*	with this receive.
*
* - COMP
*	This marks that stop sequence has been sent from the master, transfer
*	is about to terminate. However, for receiving, the master may have
*	written us some data, so receive that first.
*
*	It is an error if the amount of transferred data is less than expected.
*
* - NAK
*	This marks that master does not want our data. It is for send only.
*
* - Other interrupts
*	These interrupts are marked as error.
*
*
* @param	InstancePtr is a pointer to the XIicPs instance.
*
* @return	None.
*
* @note 	None.
*
****************************************************************************/
void XIicPs_SlaveInterruptHandler(XIicPs *InstancePtr)
{
	u32 IntrStatusReg;
	u32 IsSend = 0U;
	u32 StatusEvent = 0U;
	s32 LeftOver;
	UINTPTR BaseAddr;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);

	BaseAddr = InstancePtr->Config.BaseAddress;

	/*
	 * Read the Interrupt status register.
	 */
	IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);

	/*
	 * Write the status back to clear the interrupts so no events are missed
	 * while processing this interrupt.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, IntrStatusReg);

	/*
	 * Use the Mask register AND with the Interrupt Status register so
	 * disabled interrupts are not processed.
	 */
	IntrStatusReg &= ~(XIicPs_ReadReg(BaseAddr, XIICPS_IMR_OFFSET));

	/*
	 * Determine whether the device is sending.
	 */
	if (InstancePtr->RecvBufferPtr == NULL) {
		IsSend = 1U;
	}

	/* Data interrupt
	 *
	 * This means master wants to do more data transfers.
	 * Also check for completion of transfer, signal upper layer if done.
	 */
	if ((u32)0U != (IntrStatusReg & XIICPS_IXR_DATA_MASK)) {
		if (IsSend != 0x0U) {
			(void)TransmitFifoFill(InstancePtr);
		} else {
			LeftOver = SlaveRecvData(InstancePtr);

			/* We may finish the receive here */
			if (LeftOver == 0) {
				StatusEvent |= XIICPS_EVENT_COMPLETE_RECV;
			}
		}
	}

	/*
	 * Complete interrupt.
	 *
	 * In slave mode, it means the master has done with this transfer, so
	 * we signal the application using completion event.
	 */
	if (0U != (IntrStatusReg & XIICPS_IXR_COMP_MASK)) {
		if (IsSend != 0x0U) {
			if (InstancePtr->SendByteCount > 0) {
				StatusEvent |= XIICPS_EVENT_ERROR;
			}else {
				StatusEvent |= XIICPS_EVENT_COMPLETE_SEND;
			}
		} else {
			LeftOver = SlaveRecvData(InstancePtr);
			if (LeftOver > 0) {
				StatusEvent |= XIICPS_EVENT_ERROR;
			} else {
				StatusEvent |= XIICPS_EVENT_COMPLETE_RECV;
			}
		}
	}

	/*
	 * Nack interrupt, pass this information to application.
	 */
	if (0U != (IntrStatusReg & XIICPS_IXR_NACK_MASK)) {
		StatusEvent |= XIICPS_EVENT_NACK;
	}

	/*
	 * Timeout interrupt, pass this information to application.
	 */
	if (0U != (IntrStatusReg & XIICPS_IXR_TO_MASK)) {
		XIicPs_DisableInterrupts(BaseAddr, XIICPS_IXR_TO_MASK);
		StatusEvent |= XIICPS_EVENT_TIME_OUT;
	}

	/*
	 * All other interrupts are treated as error.
	 */
	if (0U != (IntrStatusReg & (XIICPS_IXR_RX_UNF_MASK |
				XIICPS_IXR_TX_OVR_MASK |
				XIICPS_IXR_RX_OVR_MASK))){

		StatusEvent |= XIICPS_EVENT_ERROR;
	}

	/*
	 * Signal application if there are any events.
	 */
	if ((u32)0U != StatusEvent) {
		InstancePtr->StatusHandler(InstancePtr->CallBackRef,
					   StatusEvent);
	}
}
/** @} */
