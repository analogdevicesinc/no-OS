/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xiicps_master.c
* @addtogroup Overview
* @{
*
* The xiicps_master.c file handles master mode transfers.
*
* <pre> MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---  -------- ---------------------------------------------
* 1.00a jz   01/30/10 First release
* 1.00a sdm  09/21/11 Updated the XIicPs_SetupMaster to not check for
*		      Bus Busy condition when the Hold Bit is set.
* 1.01a sg   03/30/12 Fixed an issue in XIicPs_MasterSendPolled where a
*		      check for transfer completion is added, which indicates
*			 the completion of current transfer.
* 2.0   hk   03/07/14 Added check for error status in the while loop that
*                     checks for completion. CR# 762244, 764875.
* 2.1   hk   04/24/14 Fix for CR# 789821 to handle >14 byte transfers.
*                     Fix for CR# 761060 - provision for repeated start.
* 2.2   hk   08/23/14 Slave monitor mode changes - clear FIFO, enable
*                     read mode and clear transfer size register.
*                     Disable NACK to avoid interrupts on each retry.
* 2.3	sk	 10/06/14 Fill transmit fifo before address register when sending.
* 					  Replaced XIICPS_DATA_INTR_DEPTH with XIICPS_FIFO_DEPTH.
* 					  Repeated start feature removed.
* 3.0	sk	 12/06/14 Implemented Repeated start feature.
*			 01/31/15 Modified the code according to MISRAC 2012 Compliant.
*			 02/18/15 Implemented larger data transfer using repeated start
*					  in Zynq UltraScale MP.
* 3.3   kvn 05/05/16 Modified latest code for MISRA-C:2012 Compliance.
* 3.6   ask 09/03/18 In XIicPs_MasterRecvPolled, set transfer size register
* 		     before slave address. Fix for CR996440.
* 3.8   sd 09/06/18  Enable the Timeout interrupt
* 3.9   sg 03/09/19  Added arbitration lost support in polled transfer
* 3.11  rna 12/20/19 Clear the ISR before enabling interrupts in Send/Receive.
*           12/23/19 Add 10 bit address support for Master/Slave
*           12/24/19 Disable slave monitor with XIICPS_CR_NEA_MASK for 10 bit
*                    addresses according to the IP spec.
* 3.11  sd  02/06/20 Added clocking support.
* 3.11  rna 02/12/20 Moved static data transfer functions to xiicps_xfer.c file
*	    02/18/20 Modified latest code for MISRA-C:2012 Compliance.
* 3.13  rna 11/24/20 Added timeout to XIicPs_MasterSendPolled function.
*	rna 12/17/20 Clear hold bit at correct time in Rx path of ISR
*	rna 05/24/21 Fix Misra c violations
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xiicps.h"
#include "xiicps_xfer.h"
#include "sleep.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
#define TX_MAX_LOOPCNT 1000000U	/**< Used to wait in polled function */

/************************** Function Prototypes ******************************/

/************************* Variable Definitions *****************************/

/*****************************************************************************/
/**
* @brief
* This function initiates an interrupt-driven send in master mode.
*
* It tries to send the first FIFO-full of data, then lets the interrupt
* handler to handle the rest of the data if there is any.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the send buffer.
* @param	ByteCount is the number of bytes to be sent.
* @param	SlaveAddr is the address of the slave we are sending to.
*
* @return	None.
*
* @note		This send routine is for interrupt-driven transfer only.
*
 ****************************************************************************/
void XIicPs_MasterSend(XIicPs *InstancePtr, u8 *MsgPtr, s32 ByteCount,
		 u16 SlaveAddr)
{
	UINTPTR BaseAddr;
	u32 Platform = XGetPlatform_Info();

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(MsgPtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);
	Xil_AssertVoid((u16)XIICPS_ADDR_MASK >= SlaveAddr);

	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->SendBufferPtr = MsgPtr;
	InstancePtr->SendByteCount = ByteCount;
	InstancePtr->RecvBufferPtr = NULL;
	InstancePtr->IsSend = 1;

#if defined  (XCLOCKING)
	if (InstancePtr->IsClkEnabled == 0) {
		Xil_ClockEnable(InstancePtr->Config.RefClk);
		InstancePtr->IsClkEnabled = 1;
	}
#endif
	/*
	 * Set repeated start if sending more than FIFO of data.
	 */
	if (((InstancePtr->IsRepeatedStart) != 0)||
		(ByteCount > XIICPS_FIFO_DEPTH)) {
		XIicPs_WriteReg(BaseAddr, (u32)XIICPS_CR_OFFSET,
			XIicPs_ReadReg(BaseAddr, (u32)XIICPS_CR_OFFSET) |
				(u32)XIICPS_CR_HOLD_MASK);
	}

	/*
	 * Setup as a master sending role.
	 */
	(void)XIicPs_SetupMaster(InstancePtr, SENDING_ROLE);

	(void)TransmitFifoFill(InstancePtr);

	/*
	 * Clear the interrupt status register before use it to monitor.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, XIICPS_IXR_ALL_INTR_MASK);

	XIicPs_EnableInterrupts(BaseAddr,
		(u32)XIICPS_IXR_NACK_MASK | (u32)XIICPS_IXR_COMP_MASK |
		(u32)XIICPS_IXR_ARB_LOST_MASK | (u32)XIICPS_IXR_TO_MASK);

	/*
	 * Do the address transfer to notify the slave.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_ADDR_OFFSET, (u32)SlaveAddr);


	/* Clear the Hold bit in ZYNQ if receive byte count is less than
	 * the FIFO depth to get the completion interrupt properly.
	 */
	if ((ByteCount < XIICPS_FIFO_DEPTH) && (Platform == (u32)XPLAT_ZYNQ))
	{
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(BaseAddr, (u32)XIICPS_CR_OFFSET) &
				(~(u32)XIICPS_CR_HOLD_MASK));
	}

}

/*****************************************************************************/
/**
* @brief
* This function initiates an interrupt-driven receive in master mode.
*
* It sets the transfer size register so the slave can send data to us.
* The rest of the work is managed by interrupt handler.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the receive buffer.
* @param	ByteCount is the number of bytes to be received.
* @param	SlaveAddr is the address of the slave we are receiving from.
*
* @return	None.
*
* @note		This receive routine is for interrupt-driven transfer only.
*
****************************************************************************/
void XIicPs_MasterRecv(XIicPs *InstancePtr, u8 *MsgPtr, s32 ByteCount,
		 u16 SlaveAddr)
{
	UINTPTR BaseAddr;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(MsgPtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);
	Xil_AssertVoid((u16)XIICPS_ADDR_MASK >= SlaveAddr);

	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->RecvBufferPtr = MsgPtr;
	InstancePtr->RecvByteCount = ByteCount;
	InstancePtr->SendBufferPtr = NULL;
	InstancePtr->IsSend = 0;

#if defined  (XCLOCKING)
	if (InstancePtr->IsClkEnabled == 0) {
		Xil_ClockEnable(InstancePtr->Config.RefClk);
		InstancePtr->IsClkEnabled = 1;
	}
#endif

	if ((ByteCount > XIICPS_FIFO_DEPTH) ||
		((InstancePtr->IsRepeatedStart) !=0))
	{
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(BaseAddr, (u32)XIICPS_CR_OFFSET) |
						(u32)XIICPS_CR_HOLD_MASK);
	}

	/*
	 * Initialize for a master receiving role.
	 */
	(void)XIicPs_SetupMaster(InstancePtr, RECVING_ROLE);
	/*
	 * Setup the transfer size register so the slave knows how much
	 * to send to us.
	 */
	if (ByteCount > (s32)XIICPS_MAX_TRANSFER_SIZE) {
		XIicPs_WriteReg(BaseAddr, XIICPS_TRANS_SIZE_OFFSET,
				XIICPS_MAX_TRANSFER_SIZE);
		InstancePtr->CurrByteCount = (s32)XIICPS_MAX_TRANSFER_SIZE;
		InstancePtr->UpdateTxSize = 1;
	}else {
		InstancePtr->CurrByteCount = ByteCount;
		XIicPs_WriteReg(BaseAddr, (u32)(XIICPS_TRANS_SIZE_OFFSET),
			 (u32)ByteCount);
		InstancePtr->UpdateTxSize = 0;
	}

	/*
	 * Clear the interrupt status register before use it to monitor.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, XIICPS_IXR_ALL_INTR_MASK);

	XIicPs_EnableInterrupts(BaseAddr,
		(u32)XIICPS_IXR_NACK_MASK | (u32)XIICPS_IXR_DATA_MASK |
		(u32)XIICPS_IXR_RX_OVR_MASK | (u32)XIICPS_IXR_COMP_MASK |
		(u32)XIICPS_IXR_ARB_LOST_MASK | (u32)XIICPS_IXR_TO_MASK);
	/*
	 * Do the address transfer to signal the slave.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_ADDR_OFFSET, (u32)SlaveAddr);

}

/*****************************************************************************/
/**
* @brief
* This function initiates a polled mode send in master mode.
*
* It sends data to the FIFO and waits for the slave to pick them up.
* If master fails to send data due arbitration lost, will stop transfer
* and with arbitration lost status
* If slave fails to remove data from FIFO, the send fails with
* time out.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the send buffer.
* @param	ByteCount is the number of bytes to be sent.
* @param	SlaveAddr is the address of the slave we are sending to.
*
* @return
*		- XST_SUCCESS if everything went well.
*		- XST_FAILURE if timed out.
*		- XST_IIC_ARB_LOST if arbitration lost
*
* @note		This send routine is for polled mode transfer only.
*
****************************************************************************/
s32 XIicPs_MasterSendPolled(XIicPs *InstancePtr, u8 *MsgPtr,
		 s32 ByteCount, u16 SlaveAddr)
{
	u32 IntrStatusReg;
	u32 StatusReg;
	UINTPTR BaseAddr;
	u32 Intrs;
	s32 Status = (s32)XST_FAILURE;
	u32 timeout = 0;
	_Bool Value;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(MsgPtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid((u16)XIICPS_ADDR_MASK >= SlaveAddr);

#if defined  (XCLOCKING)
	if (InstancePtr->IsClkEnabled == 0) {
		Xil_ClockEnable(InstancePtr->Config.RefClk);
		InstancePtr->IsClkEnabled = 1;
	}
#endif

	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->SendBufferPtr = MsgPtr;
	InstancePtr->SendByteCount = ByteCount;

	if (((InstancePtr->IsRepeatedStart) != 0) ||
		(ByteCount > XIICPS_FIFO_DEPTH)) {
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(BaseAddr, (u32)XIICPS_CR_OFFSET) |
						(u32)XIICPS_CR_HOLD_MASK);
	}

	(void)XIicPs_SetupMaster(InstancePtr, SENDING_ROLE);

	/*
	 * Intrs keeps all the error-related interrupts.
	 */
	Intrs = (u32)XIICPS_IXR_ARB_LOST_MASK | (u32)XIICPS_IXR_TX_OVR_MASK |
		(u32)XIICPS_IXR_NACK_MASK;

	/*
	 * Clear the interrupt status register before use it to monitor.
	 */
	IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, IntrStatusReg);

	/*
	 * Transmit first FIFO full of data.
	 */
	(void)TransmitFifoFill(InstancePtr);

	XIicPs_WriteReg(BaseAddr, XIICPS_ADDR_OFFSET, (u32)SlaveAddr);

	IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);

	/*
	 * Continue sending as long as there is more data and
	 * there are no errors.
	 */
	Value = ((InstancePtr->SendByteCount > (s32)0) &&
		((IntrStatusReg & Intrs) == (u32)0U));
	while (Value != FALSE) {
		StatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_SR_OFFSET);

		/*
		 * Wait until transmit FIFO is empty.
		 */
		if ((StatusReg & XIICPS_SR_TXDV_MASK) != 0U) {
			IntrStatusReg = XIicPs_ReadReg(BaseAddr,
					XIICPS_ISR_OFFSET);
			Value = ((InstancePtr->SendByteCount > (s32)0) &&
				((IntrStatusReg & Intrs) == (u32)0U));
			continue;
		}

		/*
		 * Send more data out through transmit FIFO.
		 */
		(void)TransmitFifoFill(InstancePtr);
		Value = ((InstancePtr->SendByteCount > (s32)0) &&
			((IntrStatusReg & Intrs) == (u32)0U));
	}

	/*
	 * Check for completion of transfer.
	 */
	while ((IntrStatusReg & XIICPS_IXR_COMP_MASK) != XIICPS_IXR_COMP_MASK){

		IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
		/*
		 * If there is an error, tell the caller.
		 */
		if ((IntrStatusReg & Intrs) != 0U) {
			if ((IntrStatusReg & XIICPS_IXR_ARB_LOST_MASK) != 0U) {
				Status = (s32) XST_IIC_ARB_LOST;
			}
			break;
		}
		/*
		 * Timeout if stuck for more than 1 second
		 */
		usleep(1);
		timeout++;
		if (timeout == TX_MAX_LOOPCNT) {
			break;
		}
	}

	if (InstancePtr->IsRepeatedStart == 0) {
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(BaseAddr,XIICPS_CR_OFFSET) &
						(~XIICPS_CR_HOLD_MASK));
	}

	/* Set the Status for XST_SUCCESS */
	if (((IntrStatusReg & Intrs) == 0U) && (timeout != TX_MAX_LOOPCNT)) {
		Status = (s32)XST_SUCCESS;
	}

	return Status;
}

/*****************************************************************************/
/**
* @brief
* This function initiates a polled mode receive in master mode.
*
* It repeatedly sets the transfer size register so the slave can
* send data to us. It polls the data register for data to come in.
* If master fails to read data due arbitration lost, will return
* with arbitration lost status.
* If slave fails to send us data, it fails with time out.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	MsgPtr is the pointer to the receive buffer.
* @param	ByteCount is the number of bytes to be received.
* @param	SlaveAddr is the address of the slave we are receiving from.
*
* @return
*		- XST_SUCCESS if everything went well.
*		- XST_FAILURE if timed out.
*		- XST_IIC_ARB_LOST if arbitration lost
*
* @note		This receive routine is for polled mode transfer only.
*
****************************************************************************/
s32 XIicPs_MasterRecvPolled(XIicPs *InstancePtr, u8 *MsgPtr,
				s32 ByteCount, u16 SlaveAddr)
{
	u32 IntrStatusReg;
	u32 Intrs;
	UINTPTR BaseAddr;
	s32 Result;
	s32 IsHold;
	s32 UpdateTxSize = 0;
	s32 ByteCountVar = ByteCount;
	u32 Platform;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(MsgPtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid((u16)XIICPS_ADDR_MASK >= SlaveAddr);

	BaseAddr = InstancePtr->Config.BaseAddress;
	InstancePtr->RecvBufferPtr = MsgPtr;
	InstancePtr->RecvByteCount = ByteCountVar;

#if defined  (XCLOCKING)
	if (InstancePtr->IsClkEnabled == 0) {
		Xil_ClockEnable(InstancePtr->Config.RefClk);
		InstancePtr->IsClkEnabled = 1;
	}
#endif

	Platform = XGetPlatform_Info();

	if((ByteCountVar > XIICPS_FIFO_DEPTH) ||
		((InstancePtr->IsRepeatedStart) !=0))
	{
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(BaseAddr, (u32)XIICPS_CR_OFFSET) |
						(u32)XIICPS_CR_HOLD_MASK);
		IsHold = 1;
	} else {
		IsHold = 0;
	}

	(void)XIicPs_SetupMaster(InstancePtr, RECVING_ROLE);

	/*
	 * Clear the interrupt status register before use it to monitor.
	 */
	IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
	XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, IntrStatusReg);


	/*
	 * Set up the transfer size register so the slave knows how much
	 * to send to us.
	 */
	if (ByteCountVar > (s32)XIICPS_MAX_TRANSFER_SIZE) {
		XIicPs_WriteReg(BaseAddr, XIICPS_TRANS_SIZE_OFFSET,
				XIICPS_MAX_TRANSFER_SIZE);
		ByteCountVar = (s32)XIICPS_MAX_TRANSFER_SIZE;
		UpdateTxSize = 1;
	}else {
		XIicPs_WriteReg(BaseAddr, XIICPS_TRANS_SIZE_OFFSET,
			 ByteCountVar);
	}


	XIicPs_WriteReg(BaseAddr, XIICPS_ADDR_OFFSET, SlaveAddr);

	/*
	 * Intrs keeps all the error-related interrupts.
	 */
	Intrs = (u32)XIICPS_IXR_ARB_LOST_MASK | (u32)XIICPS_IXR_RX_OVR_MASK |
			(u32)XIICPS_IXR_RX_UNF_MASK | (u32)XIICPS_IXR_NACK_MASK;
	/*
	 * Poll the interrupt status register to find the errors.
	 */
	IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);

	while ((InstancePtr->RecvByteCount > 0) &&
			((IntrStatusReg & Intrs) == 0U)) {

		while ((XIicPs_RxDataValid(InstancePtr)) != 0U) {
			if ((InstancePtr->RecvByteCount <
				XIICPS_DATA_INTR_DEPTH) && (IsHold != 0) &&
				(InstancePtr->IsRepeatedStart == 0) &&
				(UpdateTxSize == 0)) {
				IsHold = 0;
				XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
						XIicPs_ReadReg(BaseAddr,
						XIICPS_CR_OFFSET) &
						(~XIICPS_CR_HOLD_MASK));
			}
			XIicPs_RecvByte(InstancePtr);
		    ByteCountVar --;

			if (Platform == (u32)XPLAT_ZYNQ) {
			    if ((UpdateTxSize != 0) &&
				    (ByteCountVar == (XIICPS_FIFO_DEPTH + 1))) {
				    break;
				}
			}
		}
		if (Platform == (u32)XPLAT_ZYNQ) {
			if ((UpdateTxSize != 0) &&
				(ByteCountVar == (XIICPS_FIFO_DEPTH + 1))) {
			    /*  wait while fifo is full */
			while (XIicPs_RxFIFOFull(InstancePtr, ByteCountVar) != 0U) { ;
				}
				if ((InstancePtr->RecvByteCount - XIICPS_FIFO_DEPTH) >
					(s32)XIICPS_MAX_TRANSFER_SIZE) {

					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						XIICPS_MAX_TRANSFER_SIZE);
				    ByteCountVar = (s32)XIICPS_MAX_TRANSFER_SIZE +
							XIICPS_FIFO_DEPTH;
				} else {
					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						InstancePtr->RecvByteCount -
						XIICPS_FIFO_DEPTH);
					UpdateTxSize = 0;
				    ByteCountVar = InstancePtr->RecvByteCount;
				}
			}
		} else {
		    if ((InstancePtr->RecvByteCount > 0) && (ByteCountVar == 0)) {
				/*
				 * Clear the interrupt status register before use it to
				 * monitor.
				 */
				IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
				XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, IntrStatusReg);

				XIicPs_WriteReg(BaseAddr, XIICPS_ADDR_OFFSET, SlaveAddr);

				if ((InstancePtr->RecvByteCount) >
					(s32)XIICPS_MAX_TRANSFER_SIZE) {

					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						XIICPS_MAX_TRANSFER_SIZE);
				    ByteCountVar = (s32)XIICPS_MAX_TRANSFER_SIZE;
				} else {
					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						InstancePtr->RecvByteCount);
					UpdateTxSize = 0;
				    ByteCountVar = InstancePtr->RecvByteCount;
				}
			}
		}

		IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
	}

	if (InstancePtr->IsRepeatedStart == 0) {
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(BaseAddr,XIICPS_CR_OFFSET) &
						(~XIICPS_CR_HOLD_MASK));
	}

	if ((IntrStatusReg & Intrs) != 0U) {
		if ((IntrStatusReg & XIICPS_IXR_ARB_LOST_MASK) != 0U) {
			Result = (s32) XST_IIC_ARB_LOST;
		} else {
			Result = (s32)XST_FAILURE;
		}
	} else {
		Result = (s32)XST_SUCCESS;
	}

	return Result;
}

/*****************************************************************************/
/**
* @brief
* This function enables the slave monitor mode.
*
* It enables slave monitor in the control register and enables
* slave ready interrupt. It then does an address transfer to slave.
* Interrupt handler will signal the caller if slave responds to
* the address transfer.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
* @param	SlaveAddr is the address of the slave we want to contact.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void XIicPs_EnableSlaveMonitor(XIicPs *InstancePtr, u16 SlaveAddr)
{
	UINTPTR BaseAddr;
	u32 ConfigReg;

	Xil_AssertVoid(InstancePtr != NULL);

	BaseAddr = InstancePtr->Config.BaseAddress;

#if defined  (XCLOCKING)
	if (InstancePtr->IsClkEnabled == 0) {
		Xil_ClockEnable(InstancePtr->Config.RefClk);
		InstancePtr->IsClkEnabled = 1;
	}
#endif

	/* Clear transfer size register */
	XIicPs_WriteReg(BaseAddr, (u32)XIICPS_TRANS_SIZE_OFFSET, 0x0U);

	/*
	 * Enable slave monitor mode in control register.
	 */
	ConfigReg = XIicPs_ReadReg(BaseAddr, (u32)XIICPS_CR_OFFSET);
	ConfigReg |= (u32)XIICPS_CR_MS_MASK | (u32)XIICPS_CR_CLR_FIFO_MASK |
			(u32)XIICPS_CR_SLVMON_MASK;
	ConfigReg &= ~((u32)XIICPS_CR_RD_WR_MASK);

	/*
	 * Check if 10 bit address option is set.
	 */
	if (InstancePtr->Is10BitAddr == 1) {
		ConfigReg &= ~((u32)XIICPS_CR_NEA_MASK);
	} else {
		ConfigReg |= (u32)(XIICPS_CR_NEA_MASK);
	}

	XIicPs_WriteReg(BaseAddr, (u32)XIICPS_CR_OFFSET, ConfigReg);

	/*
	 * Set up interrupt flag for slave monitor interrupt.
	 * Don't enable NACK.
	 */
	XIicPs_EnableInterrupts(BaseAddr, (u32)XIICPS_IXR_SLV_RDY_MASK);

	/*
	 * Initialize the slave monitor register.
	 */
	XIicPs_WriteReg(BaseAddr, (u32)XIICPS_SLV_PAUSE_OFFSET, 0xFU);

	/*
	 * Set the slave address to start the slave address transmission.
	 */
	XIicPs_WriteReg(BaseAddr, (u32)XIICPS_ADDR_OFFSET, (u32)SlaveAddr);

	return;
}

/*****************************************************************************/
/**
* @brief
* This function disables slave monitor mode.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
*
* @return	None.
*
* @note		None.
*
****************************************************************************/
void XIicPs_DisableSlaveMonitor(XIicPs *InstancePtr)
{
	UINTPTR BaseAddr;
	u32 ControlReg;

	Xil_AssertVoid(InstancePtr != NULL);

	BaseAddr = InstancePtr->Config.BaseAddress;

	/*
	 * Read the control register, check the NEA bit.
	 * If 10 bit address mode is enabled, it has to be
	 * cleared to make the controller go from slave monitor
	 * mode into normal mode according to the IP document.
	 */
	ControlReg = XIicPs_ReadReg(BaseAddr, XIICPS_CR_OFFSET);
	if (((ControlReg) & (XIICPS_CR_NEA_MASK)) == 0U) {
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				ControlReg | (XIICPS_CR_NEA_MASK));
	}

	/*
	 * Clear slave monitor control bit.
	 */
	XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
			XIicPs_ReadReg(BaseAddr, XIICPS_CR_OFFSET)
					& (~XIICPS_CR_SLVMON_MASK));

	/*
	 * wait for slv monitor control bit to be clear
	 */
	while ((XIicPs_ReadReg(BaseAddr, XIICPS_CR_OFFSET)
				& XIICPS_CR_SLVMON_MASK) != 0U) {
		;
	}

	/*
	 * Write back the previous value of NEA bit in control register,
	 * in case it is modified above.
	 */
	if (((ControlReg) & (XIICPS_CR_NEA_MASK)) == 0U) {
		XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
				XIicPs_ReadReg(BaseAddr, XIICPS_CR_OFFSET)
						& (~XIICPS_CR_NEA_MASK));
	}

	/*
	 * Clear interrupt flag for slave monitor interrupt.
	 */
	XIicPs_DisableInterrupts(BaseAddr, XIICPS_IXR_SLV_RDY_MASK);
#if defined  (XCLOCKING)
	if (InstancePtr->IsClkEnabled == 1) {
		Xil_ClockDisable(InstancePtr->Config.RefClk);
		InstancePtr->IsClkEnabled = 0;
	}
#endif

	return;
}

/*****************************************************************************/
/**
* The interrupt handler for the master mode. It does the protocol handling for
* the interrupt-driven transfers.
*
* Completion events and errors are signaled to upper layer for proper handling.
*
* The interrupts that are handled are:
* - DATA
*	This case is handled only for master receive data.
*	The master has to request for more data (if there is more data to
*	receive) and read the data from the FIFO .
*
* - COMP
*	If the Master is transmitting data and there is more data to be
*	sent then the data is written to the FIFO. If there is no more data to
*	be transmitted then a completion event is signalled to the upper layer
*	by calling the callback handler.
*
*	If the Master is receiving data then the data is read from the FIFO and
*	the Master has to request for more data (if there is more data to
*	receive). If all the data has been received then a completion event
*	is signalled to the upper layer by calling the callback handler.
*	It is an error if the amount of received data is more than expected.
*
* - NAK and SLAVE_RDY
*	This is signalled to the upper layer by calling the callback handler.
*
* - All Other interrupts
*	These interrupts are marked as error. This is signalled to the upper
*	layer by calling the callback handler.
*
* @param	InstancePtr is a pointer to the XIicPs instance.
*
* @return	None.
*
* @note 	None.
*
****************************************************************************/
void XIicPs_MasterInterruptHandler(XIicPs *InstancePtr)
{
	u32 IntrStatusReg;
	u32 StatusEvent = 0U;
	UINTPTR BaseAddr;
	u16 SlaveAddr;
	s32 ByteCnt;
	s32 IsHold;
	u32 Platform;

	/*
	 * Assert validates the input arguments.
	 */
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == (u32)XIL_COMPONENT_IS_READY);

	BaseAddr = InstancePtr->Config.BaseAddress;

	Platform = XGetPlatform_Info();

	/*
	 * Read the Interrupt status register.
	 */
	IntrStatusReg = XIicPs_ReadReg(BaseAddr,
					 (u32)XIICPS_ISR_OFFSET);

	/*
	 * Write the status back to clear the interrupts so no events are
	 * missed while processing this interrupt.
	 */
	XIicPs_WriteReg(BaseAddr, (u32)XIICPS_ISR_OFFSET, IntrStatusReg);

	/*
	 * Use the Mask register AND with the Interrupt Status register so
	 * disabled interrupts are not processed.
	 */
	IntrStatusReg &= ~(XIicPs_ReadReg(BaseAddr, (u32)XIICPS_IMR_OFFSET));

	ByteCnt = InstancePtr->CurrByteCount;

	IsHold = 0;
	if ((XIicPs_ReadReg(BaseAddr, (u32)XIICPS_CR_OFFSET) & (u32)XIICPS_CR_HOLD_MASK) != 0U) {
		IsHold = 1;
	}

	/*
	 * Send
	 */
	if (((InstancePtr->IsSend) != 0) &&
		((u32)0U != (IntrStatusReg & (u32)XIICPS_IXR_COMP_MASK))) {
		if (InstancePtr->SendByteCount > 0) {
			MasterSendData(InstancePtr);
		} else {
			StatusEvent |= XIICPS_EVENT_COMPLETE_SEND;
		}
	}


	/*
	 * Receive
	 */
	if ((InstancePtr->IsSend == 0) &&
		((0U != (IntrStatusReg & (u32)XIICPS_IXR_DATA_MASK)) ||
		 (0U != (IntrStatusReg & (u32)XIICPS_IXR_COMP_MASK)))){

		while ((XIicPs_RxDataValid(InstancePtr)) != 0U) {

			XIicPs_RecvByte(InstancePtr);
			ByteCnt--;

			/* Clear hold bit when not required */
			if ((InstancePtr->RecvByteCount <=
					XIICPS_DATA_INTR_DEPTH) && (IsHold != 0)
					&& (InstancePtr->IsRepeatedStart == 0)
					&& (InstancePtr->UpdateTxSize == 0)) {
				IsHold = 0;
				XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
						XIicPs_ReadReg(BaseAddr,
						XIICPS_CR_OFFSET) &
						(~XIICPS_CR_HOLD_MASK));
			}

			if (Platform == (u32)XPLAT_ZYNQ) {
			    if ((InstancePtr->UpdateTxSize != 0) &&
				    (ByteCnt == (XIICPS_FIFO_DEPTH + 1))) {
				    break;
				}
			}
		}

		if (Platform == (u32)XPLAT_ZYNQ) {
			if ((InstancePtr->UpdateTxSize != 0) &&
				(ByteCnt == (XIICPS_FIFO_DEPTH + 1))) {

				/* wait while fifo is full */
				while (XIicPs_RxFIFOFull(InstancePtr, ByteCnt) != 0U) { ;
				}

				if ((InstancePtr->RecvByteCount - XIICPS_FIFO_DEPTH) >
					(s32)XIICPS_MAX_TRANSFER_SIZE) {

					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						XIICPS_MAX_TRANSFER_SIZE);
					ByteCnt = (s32)XIICPS_MAX_TRANSFER_SIZE +
							XIICPS_FIFO_DEPTH;
				} else {
					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						InstancePtr->RecvByteCount -
						XIICPS_FIFO_DEPTH);
					InstancePtr->UpdateTxSize = 0;
					ByteCnt = InstancePtr->RecvByteCount;
				}
			}
		} else {
			if ((InstancePtr->RecvByteCount > 0) && (ByteCnt == 0)) {
				/*
				 * Clear the interrupt status register before use it to
				 * monitor.
				 */
				IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
				XIicPs_WriteReg(BaseAddr, XIICPS_ISR_OFFSET, IntrStatusReg);

				SlaveAddr = (u16)XIicPs_ReadReg(BaseAddr, (u32)XIICPS_ADDR_OFFSET);
				XIicPs_WriteReg(BaseAddr, XIICPS_ADDR_OFFSET, SlaveAddr);

				if ((InstancePtr->RecvByteCount) >
					(s32)XIICPS_MAX_TRANSFER_SIZE) {

					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						XIICPS_MAX_TRANSFER_SIZE);
					ByteCnt = (s32)XIICPS_MAX_TRANSFER_SIZE;
				} else {
					XIicPs_WriteReg(BaseAddr,
						XIICPS_TRANS_SIZE_OFFSET,
						InstancePtr->RecvByteCount);
					InstancePtr->UpdateTxSize = 0;
					ByteCnt = InstancePtr->RecvByteCount;
				}
				XIicPs_EnableInterrupts(BaseAddr,
					(u32)XIICPS_IXR_NACK_MASK | (u32)XIICPS_IXR_DATA_MASK |
					(u32)XIICPS_IXR_RX_OVR_MASK | (u32)XIICPS_IXR_COMP_MASK |
					(u32)XIICPS_IXR_ARB_LOST_MASK);
			}
		}
		InstancePtr->CurrByteCount = ByteCnt;
	}

	if ((InstancePtr->IsSend == 0) &&
		(0U != (IntrStatusReg & XIICPS_IXR_COMP_MASK))) {
		/*
		 * If all done, tell the application.
		 */
		if (InstancePtr->RecvByteCount == 0){
			if (InstancePtr->IsRepeatedStart == 0) {
				XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
						XIicPs_ReadReg(BaseAddr,
						XIICPS_CR_OFFSET) &
						(~XIICPS_CR_HOLD_MASK));
			}
			StatusEvent |= XIICPS_EVENT_COMPLETE_RECV;
		}
	}


	/*
	 * Slave ready interrupt, it is only meaningful for master mode.
	 */
	if (0U != (IntrStatusReg & XIICPS_IXR_SLV_RDY_MASK)) {
		StatusEvent |= XIICPS_EVENT_SLAVE_RDY;
	}

	if (0U != (IntrStatusReg & XIICPS_IXR_NACK_MASK)) {
		 if (InstancePtr->IsRepeatedStart == 0) {
			XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
					XIicPs_ReadReg(BaseAddr,
					XIICPS_CR_OFFSET) &
					(~XIICPS_CR_HOLD_MASK));
		}
		StatusEvent |= XIICPS_EVENT_NACK;
	}

	/*
	 * Arbitration lost interrupt
	 */
	if (0U != (IntrStatusReg & XIICPS_IXR_ARB_LOST_MASK)) {
		StatusEvent |= XIICPS_EVENT_ARB_LOST;
	}

	if (0U != (IntrStatusReg & XIICPS_IXR_TO_MASK)) {
		XIicPs_DisableInterrupts(BaseAddr, XIICPS_IXR_TO_MASK);
		StatusEvent |= XIICPS_EVENT_TIME_OUT;
	}
	/*
	 * All other interrupts are treated as error.
	 */
	if (0U != (IntrStatusReg & (XIICPS_IXR_NACK_MASK |
			XIICPS_IXR_RX_UNF_MASK | XIICPS_IXR_TX_OVR_MASK |
			XIICPS_IXR_RX_OVR_MASK))) {
		if (InstancePtr->IsRepeatedStart == 0) {
			XIicPs_WriteReg(BaseAddr, XIICPS_CR_OFFSET,
					XIicPs_ReadReg(BaseAddr,
					XIICPS_CR_OFFSET) &
					(~XIICPS_CR_HOLD_MASK));
		}
		StatusEvent |= XIICPS_EVENT_ERROR;
	}

	/*
	 * Signal application if there are any events.
	 */
	if (StatusEvent != 0U) {
		InstancePtr->StatusHandler(InstancePtr->CallBackRef,
					   StatusEvent);
	}

}
/** @} */
