/******************************************************************************
* Copyright (c) 2017 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
*
* @file xil_smc.c
*
* This file contains function for initiating SMC call
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 6.2 	pkp  	 02/16/17 First release
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/
#include "xil_types.h"
#include "xil_smc.h"

#if EL1_NONSECURE
/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Function Prototypes *****************************/

/************************** Variable Definitions *****************************/
XSmc_OutVar SmcResult;

/*****************************************************************************/
/**
* @brief	Initiate SMC call to EL3 secure monitor to request for secure
*			service. This function is only applicable for EL1 Non-secure bsp.
*
* @param	FunctionID is the SMC identifier for a particular secure service
*			request
* @param	Arg1 to Arg6 is the arguments passed to EL3 secure monitor
* @param	Arg7 is Hypervisor Client ID register
*
* @return	Result from secure payload service
* @note		FunctionID and Arg1-Arg7 should be as per SMC calling convention
*
******************************************************************************/
XSmc_OutVar Xil_Smc(u64 FunctionID, u64 Arg1, u64 Arg2, u64 Arg3, u64 Arg4,
					u64 Arg5, u64 Arg6, u64 Arg7){

	/*
	 * Since registers x8 to x17 are not saved by secure monitor during SMC
	 * it must be preserved.
	 */
	XSave_X8toX17();

	/* Moving to EL3 secure monitor with smc call. */

	__asm__ __volatile__ ("smc #0x0");

	/*
	 * The result of the secure services are stored in x0 - x3. They are
	 * moved to SmcResult to return the result.
	 */
	__asm__ __volatile__("mov	x8, x0");
	__asm__ __volatile__("mov	x9, x1");
	__asm__ __volatile__("mov	x10, x2");
	__asm__ __volatile__("mov	x11, x3");

	__asm__ __volatile__("mov	%0, x8" : "=r" (SmcResult.Arg0));
	__asm__ __volatile__("mov	%0, x9" : "=r" (SmcResult.Arg1));
	__asm__ __volatile__("mov	%0, x10" : "=r" (SmcResult.Arg2));
	__asm__ __volatile__("mov	%0, x11" : "=r" (SmcResult.Arg3));

	XRestore_X8toX17();

	return SmcResult;
}
#endif
