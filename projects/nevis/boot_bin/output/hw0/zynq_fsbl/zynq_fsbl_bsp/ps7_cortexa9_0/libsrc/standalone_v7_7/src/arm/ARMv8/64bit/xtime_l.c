/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xtime_l.c
*
* This file contains low level functions to get/set time from the Global Timer
* register in the ARM Cortex A53 MP core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ------ -------- ---------------------------------------------------
* 5.00 	pkp	   05/29/14 First release
* 5.05	pkp	   04/13/16 Added XTime_StartTimer API to start the global timer
*						counter if it is disabled. Also XTime_GetTime calls
*						this API to ensure the global timer counter is enabled
* 6.02  pkp	   01/22/17 Added support for EL1 non-secure
* 7.5   mus    04/30/21  Moved pragma message from xtime_l.h to xtime_l.c, to avoid
*                        displaying same warnings multiple times. It fixes CR#1090562.
* </pre>
*
* @note		None.
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "xtime_l.h"
#include "xpseudo_asm.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"
#include "bspconfig.h"

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/
#if defined (XSLEEP_TIMER_IS_DEFAULT_TIMER)
#pragma message ("For the sleep routines, Global timer is being used")
#endif
/****************************************************************************/
/**
* @brief	Start the 64-bit physical timer counter.
*
* @return	None.
*
* @note		The timer is initialized only if it is disabled. If the timer is
*			already running this function does not perform any operation. This
*			API is effective only if BSP is built for EL3. For EL1 Non-secure,
*			it simply exits.
*
****************************************************************************/
void XTime_StartTimer(void)
{
	if (EL3 == 1){
		/* Enable the global timer counter only if it is disabled */
		if(((Xil_In32(XIOU_SCNTRS_BASEADDR + XIOU_SCNTRS_CNT_CNTRL_REG_OFFSET))
					& XIOU_SCNTRS_CNT_CNTRL_REG_EN_MASK) !=
					XIOU_SCNTRS_CNT_CNTRL_REG_EN){
			/*write frequency to System Time Stamp Generator Register*/
			Xil_Out32((XIOU_SCNTRS_BASEADDR + XIOU_SCNTRS_FREQ_REG_OFFSET),
					XIOU_SCNTRS_FREQ);
			/*Enable the timer/counter*/
			Xil_Out32((XIOU_SCNTRS_BASEADDR + XIOU_SCNTRS_CNT_CNTRL_REG_OFFSET)
						,XIOU_SCNTRS_CNT_CNTRL_REG_EN);
		}
	}
}
/****************************************************************************/
/**
* @brief	Timer of A53 runs continuously and the time can not be set as
*			desired. This API doesn't contain anything. It is defined to have
*			uniformity across platforms.
*
* @param	Xtime_Global: 64bit value to be written to the physical timer
*			counter register. Since API does not do anything, the value is
*			not utilized.
*
* @return	None.
*
****************************************************************************/
void XTime_SetTime(XTime Xtime_Global)
{
	(void) Xtime_Global;
/*As the generic timer of A53 runs constantly time can not be set as desired
so the API is left unimplemented*/
}

/****************************************************************************/
/**
* @brief	Get the time from the physical timer counter register.
*
* @param	Xtime_Global: Pointer to the 64-bit location to be updated with the
*			current value of physical timer counter register.
*
* @return	None.
*
****************************************************************************/
void XTime_GetTime(XTime *Xtime_Global)
{
	if (EL3 == 1)
	/* Start global timer counter, it will only be enabled if it is disabled */
		XTime_StartTimer();

	*Xtime_Global = mfcp(CNTPCT_EL0);
}
