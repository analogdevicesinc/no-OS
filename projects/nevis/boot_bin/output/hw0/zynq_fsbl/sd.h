/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file sd.h
*
* This file contains the interface for the Secure Digital (SD) card
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a bh	03/10/11 Initial release
* 7.00a kc  10/18/13 Integrated SD/MMC driver
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___SD_H___
#define ___SD_H___


#ifdef __cplusplus
extern "C" {
#endif


/************************** Function Prototypes ******************************/

#if defined(XPAR_PS7_SD_0_S_AXI_BASEADDR) || defined(XPAR_XSDPS_0_BASEADDR)
u32 InitSD(const char *);

u32 SDAccess( u32 SourceAddress,
		u32 DestinationAddress,
		u32 LengthWords);

void ReleaseSD(void);
#endif
/************************** Variable Definitions *****************************/
#ifdef __cplusplus
}
#endif


#endif /* ___SD_H___ */

