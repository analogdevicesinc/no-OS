/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xtime_l.h
* @addtogroup a9_time_apis Cortex A9 Time Functions
*
* xtime_l.h provides access to the 64-bit Global Counter in the PMU. This
* counter increases by one at every two processor cycles. These functions can
* be used to get/set time in the global timer.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- ---------------------------------------------------
* 1.00a rp/sdm 11/03/09 Initial release.
* 3.06a sgd    05/15/12 Updated get/set time functions to make use Global Timer
* 3.06a asa    06/17/12 Reverted back the changes to make use Global Timer.
* 3.07a sgd    07/05/12 Updated get/set time functions to make use Global Timer
* 6.6   srm    10/23/17 Updated the macros to support user configurable sleep
*						implementation
* 6.8   aru  09/06/18 Removed compilation warnings for ARMCC toolchain.
* 7.5   mus    04/30/21  Moved pragma message from xtime_l.h to xtime_l.c, to avoid
*                        displaying same warnings multiple times. It fixes CR#1090562.
* </pre>
*
******************************************************************************/

/**
*@cond nocomments
*/

#ifndef XTIME_H /* prevent circular inclusions */
#define XTIME_H /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xparameters.h"

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

typedef u64 XTime;

/************************** Constant Definitions *****************************/
#define GLOBAL_TMR_BASEADDR               XPAR_GLOBAL_TMR_BASEADDR
#define GTIMER_COUNTER_LOWER_OFFSET       0x00U
#define GTIMER_COUNTER_UPPER_OFFSET       0x04U
#define GTIMER_CONTROL_OFFSET             0x08U

#if defined (SLEEP_TIMER_BASEADDR)
#define COUNTS_PER_SECOND          (SLEEP_TIMER_FREQUENCY)
#else
/* Global Timer is always clocked at half of the CPU frequency */
#define COUNTS_PER_SECOND          (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ /2)
#endif
/************************** Variable Definitions *****************************/

/**
*@endcond
*/

/************************** Function Prototypes ******************************/

void XTime_SetTime(XTime Xtime_Global);
void XTime_GetTime(XTime *Xtime_Global);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XTIME_H */
/**
* @} End of "addtogroup a9_time_apis".
*/
