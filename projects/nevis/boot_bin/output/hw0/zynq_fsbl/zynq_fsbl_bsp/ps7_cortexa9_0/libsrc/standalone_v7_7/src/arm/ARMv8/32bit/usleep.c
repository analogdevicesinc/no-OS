/******************************************************************************
* Copyright (c) 2015 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file usleep.c
*
* This function provides a microsecond delay using the Generic counter register in
* the ARM Cortex A53 MPcore.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.2	pkp  	 28/05/15 First release
* 5.4	pkp		 28/01/16 Modified the usleep API to configure Time Stamp
*						  generator only when disable using frequency from
*						  xparamters.h instead of hardcoding
* 5.05	pkp		 13/04/16 Modified usleep routine to call XTime_StartTimer
*						  which enables timer only when it is disabled and
*						  read counter value directly from register instead
*						  of calling XTime_GetTime for optimization
* 6.0   mus      08/18/16 Updated the usleep signature. Fix for CR#956899
* 6.6	srm      10/18/17 Updated sleep routines to support user configurable
*	                  implementation. Now sleep routines will use Timer
*                         specified by the user (i.e. Global timer/TTC timer)
* 7.5   dp      01/05/21 Updated COUNTS_PER_USECOND macros to round it off to
*                        nearest possible value so that delta error in time
*                        calculations can be minimized.
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "sleep.h"
#include "xtime_l.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xpseudo_asm.h"

#if defined (SLEEP_TIMER_BASEADDR)
#include "xil_sleeptimer.h"
#endif

/****************************  Constant Definitions  ************************/

/* Global Timer is always clocked at half of the CPU frequency */
#define COUNTS_PER_USECOND  ((COUNTS_PER_SECOND + 500000)/1000000)

/*****************************************************************************/
/**
*
* This API gives a delay in microseconds
*
* @param	useconds requested
*
* @return	0.
*
* @note		None.
*
****************************************************************************/
int usleep_A53(unsigned long useconds)
{

#if defined (SLEEP_TIMER_BASEADDR)
	Xil_SleepTTCCommon(useconds, COUNTS_PER_USECOND);
#else
	XTime tEnd, tCur;
	/* Start global timer counter, it will only be enabled if it is disabled */
	XTime_StartTimer();

	tCur = arch_counter_get_cntvct();
	tEnd = tCur + (((XTime) useconds) * COUNTS_PER_USECOND);
	do
	{
		tCur = arch_counter_get_cntvct();
	} while (tCur < tEnd);
#endif

	return 0;
}
