/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xdevcfg_hw.c
* @addtogroup devcfg_v3_7
* @{
*
* This file contains the implementation of the interface reset functionality
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who Date     Changes
* ----- --- -------- ---------------------------------------------
* 2.04a kpc 10/07/13 First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xdevcfg_hw.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
* This function perform the reset sequence to the given devcfg interface by
* configuring the appropriate control bits in the devcfg specifc registers
* the devcfg reset squence involves the following steps
*	Disable all the interuupts
*	Clear the status
*	Update relevant config registers with reset values
*	Disbale the looopback mode and pcap rate enable
*
* @param   BaseAddress of the interface
*
* @return N/A
*
* @note
* This function will not modify the slcr registers that are relavant for
* devcfg controller
******************************************************************************/
void XDcfg_ResetHw(u32 BaseAddr)
{
	u32 Regval = 0;

	/* Mask the interrupts  */
	XDcfg_WriteReg(BaseAddr, XDCFG_INT_MASK_OFFSET,
			XDCFG_IXR_ALL_MASK);
	/* Clear the interuupt status */
	Regval = XDcfg_ReadReg(BaseAddr, XDCFG_INT_STS_OFFSET);
	XDcfg_WriteReg(BaseAddr, XDCFG_INT_STS_OFFSET, Regval);
	/* Clear the source address register */
	XDcfg_WriteReg(BaseAddr, XDCFG_DMA_SRC_ADDR_OFFSET, 0x0);
	/* Clear the destination address register */
	XDcfg_WriteReg(BaseAddr, XDCFG_DMA_DEST_ADDR_OFFSET, 0x0);
	/* Clear the source length register */
	XDcfg_WriteReg(BaseAddr, XDCFG_DMA_SRC_LEN_OFFSET, 0x0);
	/* Clear the destination length register */
	XDcfg_WriteReg(BaseAddr, XDCFG_DMA_DEST_LEN_OFFSET, 0x0);
	/* Clear the loopback enable bit */
	Regval = XDcfg_ReadReg(BaseAddr, XDCFG_MCTRL_OFFSET);
	Regval = Regval & ~XDCFG_MCTRL_PCAP_LPBK_MASK;
	XDcfg_WriteReg(BaseAddr, XDCFG_MCTRL_OFFSET, Regval);
	/*Reset the configuration register to reset value */
	XDcfg_WriteReg(BaseAddr, XDCFG_CFG_OFFSET,
				XDCFG_CONFIG_RESET_VALUE);
	/*Disable the PCAP rate enable bit */
	Regval = XDcfg_ReadReg(BaseAddr, XDCFG_CTRL_OFFSET);
	Regval = Regval & ~XDCFG_CTRL_PCAP_RATE_EN_MASK;
	XDcfg_WriteReg(BaseAddr, XDCFG_CTRL_OFFSET, Regval);

}
/** @} */
