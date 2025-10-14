/******************************************************************************
* Copyright (c) 2015 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************
*
* @file sleep.c
*
* This function provides a second delay using the Generic Counter register in
* the ARM Cortex A53 MPcore.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.2	pkp  	 28/05/15 First release
* 5.4	pkp		 28/01/16 Modified the sleep API to configure Time Stamp
*						  generator only when disable using frequency from
*						  xparamters.h instead of hardcoding
* 5.05	pkp		 13/04/16 Modified sleep routine to call XTime_StartTimer
*						  which enables timer only when it is disabled and
*						  read counter value directly from register instead
*						  of calling XTime_GetTime for optimization
* 6.0   mus      08/18/16 Updated the sleep signature. Fix for CR#956899
* 6.6	srm      10/18/17 Updated sleep routines to support user configurable
*			  implementation. Now sleep routines will use Timer
*                         specified by the user (i.e. Global timer/TTC timer)
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "sleep.h"
#include "xtime_l.h"
#include "xparameters.h"

#if defined (SLEEP_TIMER_BASEADDR)
#include "xil_sleeptimer.h"
#endif

/*****************************************************************************/
/**
*
* This API is used to provide delays in seconds
*
* @param	seconds requested
*
* @return	0 always
*
* @note		None.
*
****************************************************************************/
unsigned sleep_A53(unsigned int seconds)
{
#if defined (SLEEP_TIMER_BASEADDR)
	Xil_SleepTTCCommon(seconds, COUNTS_PER_SECOND);
#else
	XTime tEnd, tCur;
	/* Start global timer counter, it will only be enabled if it is disabled */
	XTime_StartTimer();

	tCur = arch_counter_get_cntvct();
	tEnd  = tCur + (((XTime) seconds) * COUNTS_PER_SECOND);
	do
	{
		tCur = arch_counter_get_cntvct();
	} while (tCur < tEnd);
#endif

	return 0;
}
