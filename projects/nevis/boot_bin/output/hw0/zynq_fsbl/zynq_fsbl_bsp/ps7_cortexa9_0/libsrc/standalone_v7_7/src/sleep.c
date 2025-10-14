/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************
*
* @file sleep.c
*
* This function provides a second delay using the Global Timer register in
* the ARM Cortex A9 MP core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 1.00a ecm/sdm  11/11/09 First release
* 3.07a sgd      07/05/12 Updated sleep function to make use Global
* 6.0   asa      08/15/16 Updated the sleep signature. Fix for CR#956899.
* 6.6	srm      10/18/17 Updated sleep routines to support user configurable
*			  implementation. Now sleep routines will use Timer
*                         specified by the user (i.e. Global timer/TTC timer)
*
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
/*
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
unsigned sleep_A9(unsigned int seconds)
{
#if defined (SLEEP_TIMER_BASEADDR)
	Xil_SleepTTCCommon(seconds, COUNTS_PER_SECOND);
#else
	XTime tEnd, tCur;

	XTime_GetTime(&tCur);
	tEnd  = tCur + (((XTime) seconds) * COUNTS_PER_SECOND);
	do
    {
		XTime_GetTime(&tCur);
    } while (tCur < tEnd);
#endif

  return 0;
}
