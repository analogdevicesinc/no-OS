/******************************************************************************
* Copyright (C) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xuartps_selftest.c
* @addtogroup uartps_v3_11
* @{
*
* This file contains the self-test functions for the XUartPs driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date	Changes
* ----- ------ -------- -----------------------------------------------
* 1.00	drg/jz 01/13/10 First Release
* 3.00  kvn    02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.9   sd     02/06/20 Added clock support
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xstatus.h"
#include "xuartps.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

#define XUARTPS_TOTAL_BYTES (u8)32

/************************** Variable Definitions *****************************/

static u8 TestString[XUARTPS_TOTAL_BYTES]="abcdefghABCDEFGH012345677654321";
static u8 ReturnString[XUARTPS_TOTAL_BYTES];

/************************** Function Prototypes ******************************/


/****************************************************************************/
/**
*
* This function runs a self-test on the driver and hardware device. This self
* test performs a local loopback and verifies data can be sent and received.
*
* The time for this test is proportional to the baud rate that has been set
* prior to calling this function.
*
* The mode and control registers are restored before return.
*
* @param	InstancePtr is a pointer to the XUartPs instance
*
* @return
*		 - XST_SUCCESS if the test was successful
*		- XST_UART_TEST_FAIL if the test failed looping back the data
*
* @note
*
* This function can hang if the hardware is not functioning properly.
*
******************************************************************************/
s32 XUartPs_SelfTest(XUartPs *InstancePtr)
{
	s32 Status = XST_SUCCESS;
	u32 IntrRegister;
	u32 ModeRegister;
	u8 Index;
	u32 ReceiveDataResult;

	/* Assert validates the input arguments */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
#if defined  (XCLOCKING)
	Xil_ClockEnable(InstancePtr->Config.RefClk);
#endif

	/* Disable all interrupts in the interrupt disable register */
	IntrRegister = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				   XUARTPS_IMR_OFFSET);
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress, XUARTPS_IDR_OFFSET,
		XUARTPS_IXR_MASK);

	/* Setup for local loopback */
	ModeRegister = XUartPs_ReadReg(InstancePtr->Config.BaseAddress,
				   XUARTPS_MR_OFFSET);
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress, XUARTPS_MR_OFFSET,
			   ((ModeRegister & (u32)(~XUARTPS_MR_CHMODE_MASK)) |
				(u32)XUARTPS_MR_CHMODE_L_LOOP));

	/* Send a number of bytes and receive them, one at a time. */
	for (Index = 0U; Index < XUARTPS_TOTAL_BYTES; Index++) {
		/*
		 * Send out the byte and if it was not sent then the failure
		 * will be caught in the comparison at the end
		 */
		(void)XUartPs_Send(InstancePtr, &TestString[Index], 1U);

		/*
		 * Wait until the byte is received. This can hang if the HW
		 * is broken. Watch for the FIFO empty flag to be false.
		 */
		ReceiveDataResult = Xil_In32((InstancePtr->Config.BaseAddress) + XUARTPS_SR_OFFSET) &
				XUARTPS_SR_RXEMPTY;
		while (ReceiveDataResult == XUARTPS_SR_RXEMPTY ) {
			ReceiveDataResult = Xil_In32((InstancePtr->Config.BaseAddress) + XUARTPS_SR_OFFSET) &
					XUARTPS_SR_RXEMPTY;
		}

		/* Receive the byte */
		(void)XUartPs_Recv(InstancePtr, &ReturnString[Index], 1U);
	}

	/*
	 * Compare the bytes received to the bytes sent to verify the exact data
	 * was received
	 */
	for (Index = 0U; Index < XUARTPS_TOTAL_BYTES; Index++) {
		if (TestString[Index] != ReturnString[Index]) {
			Status = XST_UART_TEST_FAIL;
		}
	}

	/*
	 * Restore the registers which were altered to put into polling and
	 * loopback modes so that this test is not destructive
	 */
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress, XUARTPS_IER_OFFSET,
			   IntrRegister);
	XUartPs_WriteReg(InstancePtr->Config.BaseAddress, XUARTPS_MR_OFFSET,
			   ModeRegister);

#if defined  (XCLOCKING)
	Xil_ClockDisable(InstancePtr->Config.RefClk);
#endif
	return Status;
}
/** @} */
