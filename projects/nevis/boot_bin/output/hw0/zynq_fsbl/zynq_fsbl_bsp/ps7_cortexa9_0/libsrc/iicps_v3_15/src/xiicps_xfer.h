/******************************************************************************
* Copyright (C) 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xiicps_xfer.h
* @addtogroup Overview
* @{
*
* The xiicps_xfer.h file contains implementation of required helper functions
* for the XIicPs driver.
*
* <pre> MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------  -------- --------------------------------------------
* 3.11  rna     12/10/19 First release
* </pre>
*
******************************************************************************/
/** @cond INTERNAL */
#ifndef XIICPS_XFER_H             /* prevent circular inclusions */
#define XIICPS_XFER_H             /**< by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xiicps.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/*****************************************************************************/
/**
*
* This function is to check if Rx data is valid or not.
*
* @param        InstancePtr is a pointer to the XIicPs instance.
*
* @return       The return value is '1' if Rx data is valid, '0' otherwise.
*
* @note         None.
*
******************************************************************************/
static INLINE u32 XIicPs_RxDataValid(XIicPs *InstancePtr)
{
	return ((XIicPs_ReadReg(InstancePtr->Config.BaseAddress, XIICPS_SR_OFFSET))
				& XIICPS_SR_RXDV_MASK);
}

/*****************************************************************************/
/**
*
* This function is to check if Rx FIFO is full or not.
*
* @param        InstancePtr is a pointer to the XIicPs instance.
* @param        ByteCountVar is the number of bytes to be received.
*
* @return       The return value is '0' if Rx FIFO is full, '1' otherwise.
*
* @note         None.
*
******************************************************************************/
static INLINE u32 XIicPs_RxFIFOFull(XIicPs *InstancePtr, s32 ByteCountVar)
{
	u32 Status = 0;

	Status = (u32)(XIicPs_ReadReg(InstancePtr->Config.BaseAddress,
				XIICPS_TRANS_SIZE_OFFSET) !=(u32)(ByteCountVar -
					(s32)XIICPS_FIFO_DEPTH));
	return Status;
}

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
/*
 * This function prepares a device to transfer as a master.
 */
s32 XIicPs_SetupMaster(XIicPs *InstancePtr, s32 Role);
/*
 * This function handles continuation of sending data.
 */
void MasterSendData(XIicPs *InstancePtr);
/*
 * This function handles continuation of receiving data.
 */
s32 SlaveRecvData(XIicPs *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/** @endcond */
/** @} */
