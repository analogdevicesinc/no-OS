/******************************************************************************
* Copyright (C) 2013 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xspips_hw.c
* @addtogroup spips_v3_7
* @{
*
* Contains the reset and post boot rom state initialization.
* Function prototypes in xspips_hw.h
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- -----------------------------------------------
* 1.06a hk     08/22/13 First release.
* 3.00  kvn    02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.02  raw    11/23/15 Updated XSpiPs_ResetHw() to read all RXFIFO
* 			entries. This change is to tackle CR#910231.
* 3.1   tjs    11/23/18 Added a check for A72 and R5 processor to
*                       avoid changes made for the workaround DT#842463.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xspips_hw.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Variable Definitions *****************************/


/*****************************************************************************/
/**
*
* Resets the spi module
*
* @param    BaseAddress is the base address of the device.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
void XSpiPs_ResetHw(u32 BaseAddress)
{
	u32 Check;
#if !defined(versal)
	u32 Count;
#endif

	/*
	 * Disable Interrupts
	 */
	XSpiPs_WriteReg(BaseAddress, XSPIPS_IDR_OFFSET,
			XSPIPS_IXR_DISABLE_ALL_MASK);

	/*
	 * Disable device
	 */
	XSpiPs_WriteReg(BaseAddress, XSPIPS_ER_OFFSET,
				0U);
	/*
	 * Write default value to RX and TX threshold registers
	 * RX threshold should be set to 1 here as the corresponding
	 * status bit is used to clear the FIFO next
	 */
	XSpiPs_WriteReg(BaseAddress, XSPIPS_TXWR_OFFSET,
			(XSPIPS_TXWR_RESET_VALUE & XSPIPS_TXWR_MASK));
	XSpiPs_WriteReg(BaseAddress, XSPIPS_RXWR_OFFSET,
			(XSPIPS_RXWR_RESET_VALUE & XSPIPS_RXWR_MASK));

	/*
	 * Clear RXFIFO
	 */
	Check = (XSpiPs_ReadReg(BaseAddress,XSPIPS_SR_OFFSET) &
		XSPIPS_IXR_RXNEMPTY_MASK);
	while (Check != 0U) {
		(void)XSpiPs_ReadReg(BaseAddress, XSPIPS_RXD_OFFSET);
		Check = (XSpiPs_ReadReg(BaseAddress,XSPIPS_SR_OFFSET) &
			XSPIPS_IXR_RXNEMPTY_MASK);
	}

	/*
	 * Read all RXFIFO entries
	 */
#if !defined(versal)
	for (Count = 0U; Count < XSPIPS_FIFO_DEPTH; Count++) {
		(void)XSpiPs_ReadReg(BaseAddress, XSPIPS_RXD_OFFSET);
	}
#endif
	/*
	 * Clear status register by writing 1 to the write to clear bits
	 */
	XSpiPs_WriteReg(BaseAddress, XSPIPS_SR_OFFSET,
				XSPIPS_IXR_WR_TO_CLR_MASK);

	/*
	 * Write default value to configuration register
	 * De-select all slaves
	 */
	XSpiPs_WriteReg(BaseAddress, XSPIPS_CR_OFFSET,
				XSPIPS_CR_RESET_STATE |
				XSPIPS_CR_SSCTRL_MASK);

}
/** @} */
