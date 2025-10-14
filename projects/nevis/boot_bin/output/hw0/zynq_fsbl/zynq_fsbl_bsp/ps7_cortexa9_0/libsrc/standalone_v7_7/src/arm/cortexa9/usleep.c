/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file usleep.c
*
* This function provides a microsecond delay using the Global Timer register in
* the ARM Cortex A9 MP core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 1.00a ecm/sdm  11/11/09 First release
* 3.07a sgd      07/05/12 Updated micro sleep function to make use Global Timer
* 4.2	pkp		 08/04/14 Removed unimplemented nanosleep routine as it is not
*						  possible to generate timer in nanosecond due to
*						  limited cpu frequency
* 6.0   asa      08/15/16 Updated the usleep signature. Fix for CR#956899.
* 6.6	srm	 10/18/17 Updated sleep routines to support user configurable
*			  implementation. Now sleep routines will use Timer
*                         specified by the user (i.e. Global timer/TTC timer)
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "sleep.h"
#include "xtime_l.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xpseudo_asm.h"
#include "xreg_cortexa9.h"

#if defined (SLEEP_TIMER_BASEADDR)
#include "xil_sleeptimer.h"
#endif

/****************************  Constant Definitions  ************************/
#if defined (SLEEP_TIMER_BASEADDR)
#define COUNTS_PER_USECOND (SLEEP_TIMER_FREQUENCY / 1000000)
#else
/* Global Timer is always clocked at half of the CPU frequency */
#define COUNTS_PER_USECOND  (XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ / (2U*1000000U))
#endif

/*****************************************************************************/
/**
*
* This API gives a delay in microseconds
*
* @param	useconds requested
*
* @return	0 if the delay can be achieved, -1 if the requested delay
*		is out of range
*
* @note		None.
*
****************************************************************************/
int usleep_A9(unsigned long useconds)
{
#if defined (SLEEP_TIMER_BASEADDR)
	Xil_SleepTTCCommon(useconds, COUNTS_PER_USECOND);
#else
	XTime tEnd, tCur;

	XTime_GetTime(&tCur);
	tEnd = tCur + (((XTime) useconds) * COUNTS_PER_USECOND);
	do
	{
		XTime_GetTime(&tCur);
	} while (tCur < tEnd);
#endif

	return 0;
}
