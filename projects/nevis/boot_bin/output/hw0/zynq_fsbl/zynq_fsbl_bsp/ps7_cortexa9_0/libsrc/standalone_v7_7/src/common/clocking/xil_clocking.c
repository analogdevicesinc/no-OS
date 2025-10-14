/******************************************************************************
* Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_clocking.c
*
* The xil_clocking.c file contains clocking related functions and macros.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 7.2 sd  02/06/20 First release of clocking
* 7.2 sd  03/20/20 Added checking for isolation case
* </pre>
*
******************************************************************************/

#include "xil_clocking.h"
/************************** Variable Definitions *****************************/

#if defined  (XPAR_XCRPSU_0_DEVICE_ID) && defined (XCLOCKING)

XClock ClockInstance;		/* Instance of clock Controller */
XClockPs_Config *ConfigPtr;


XStatus Xil_ClockInit(void)
{
	XStatus Status = XST_FAILURE;

	/* Lookup clock configurations */
	ConfigPtr = XClock_LookupConfig(XPAR_XCLOCKPS_DEVICE_ID);

	/* Initialize the Clock controller driver */
	Status = XClock_CfgInitialize(&ClockInstance, ConfigPtr);
	return Status;
}

XStatus Xil_ClockEnable(XClock_OutputClks ClockId)
{

	XStatus Status = XST_FAILURE;

	Status = XClock_EnableClock(ClockId);
	return Status;
}

XStatus Xil_ClockDisable(XClock_OutputClks ClockId)
{
	XStatus Status = XST_FAILURE;

	Status = XClock_DisableClock(ClockId);
	return Status;
}

XStatus Xil_ClockGetRate(XClock_OutputClks ClockId, XClockRate *Rate)
{
	XStatus Status = XST_FAILURE;

	Xil_AssertNonvoid(Rate != NULL);

	Status = XClock_GetRate(ClockId, Rate);
	if (XST_SUCCESS == Status) {
		xdbg_printf(XDBG_DEBUG_GENERAL, "Operating rate =  %lx\n",*Rate);
	} else {
		xdbg_printf(XDBG_DEBUG_ERROR, "Failed: Fetching rate\r\n");
	}
	return Status;
}

XStatus Xil_ClockSetRate(XClock_OutputClks ClockId, XClockRate Rate,
							XClockRate *SetRate)
{
	XStatus Status = XST_FAILURE;

	Xil_AssertNonvoid(SetRate != NULL);

	if (Rate == 0) {
		return XST_FAILURE;
	}
	Status = XClock_SetRate(ClockId, Rate, SetRate);
	if (XST_SUCCESS != Status) {
		xdbg_printf(XDBG_DEBUG_ERROR, "Failed Setting rate\n");
	}
	return Status;
}

#else
XStatus Xil_ClockGetRate(XClock_OutputClks ClockId, XClockRate *Rate)
{
	(void) ClockId;
	(void) Rate;
	return XST_FAILURE;
}

XStatus Xil_ClockSetRate(XClock_OutputClks ClockId, XClockRate Rate,
							XClockRate *SetRate) {
	(void) ClockId;
	(void) Rate;
	(void) SetRate;
	return XST_FAILURE;
}

XStatus Xil_ClockInit(void)
{
	return XST_SUCCESS;
}

XStatus Xil_ClockEnable(XClock_OutputClks ClockId)
{
	(void) ClockId;
	return XST_SUCCESS;
}

XStatus Xil_ClockDisable(XClock_OutputClks ClockId)
{
	(void) ClockId;
	return XST_SUCCESS;
}

#endif /* XCLOCKING */
