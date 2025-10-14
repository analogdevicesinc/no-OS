/******************************************************************************
* Copyright (c) 2020 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_clocking.h
*
* The xil_clocking.h file contains clocking related functions and macros.
* certain conditions.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 7.2 sd  12/11/19 First release
* 7.2 sd  03/20/20 Added checking for isolation case
* 7.7 sk  01/10/22 Add function prototype for Xil_ClockGetRate to fix
* 		   misra_c_2012_rule_8_4 violation.
* </pre>
*
******************************************************************************/

#ifndef XIL_CLOCKING_H	/* prevent circular inclusions */
#define XIL_CLOCKING_H	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#include "xdebug.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xparameters.h"
#include "xstatus.h"
#if defined  (XPAR_XCRPSU_0_DEVICE_ID)
#include "xclockps.h"
#else
typedef u32 XClock_OutputClks;
typedef u64 XClockRate;
#endif

/***************************** Include Files *********************************/


/************************** Constant Definitions *****************************/


XStatus Xil_ClockDisable(XClock_OutputClks ClockId);
XStatus Xil_ClockEnable(XClock_OutputClks ClockId);
XStatus Xil_ClockInit(void);
XStatus Xil_ClockGet(void);
XStatus Xil_ClockSetRate(XClock_OutputClks ClockId, XClockRate Rate, XClockRate *SetRate);
XStatus Xil_ClockGetRate(XClock_OutputClks ClockId, XClockRate *Rate);

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
