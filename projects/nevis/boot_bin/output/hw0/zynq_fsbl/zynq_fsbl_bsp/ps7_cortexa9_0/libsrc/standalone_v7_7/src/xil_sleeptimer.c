/******************************************************************************
* Copyright (c) 2017 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file xil_sleeptimer.c
*
* This file provides the common helper routines for the sleep API's
*
* <pre>
* MODIFICATION HISTORY :
*
* Ver   Who  Date	 Changes
* ----- ---- -------- -------------------------------------------------------
* 6.6	srm  10/18/17 First Release.
* 6.6   srm  04/20/18 Fixed compilation warning in Xil_SleepTTCCommon API
* 7.0   mus  03/27/19 Updated XTime_StartTTCTimer to skip IOU slcr address
*                     space access, if processor is nonsecure and IOU slcr
*                     address space is secure. CR#1015725.
* 7.7	sk   01/10/22 Update values from signed to unsigned to fix
* 		      misra_c_2012_rule_10_4 violation.
* 7.7	sk   01/10/22 Add explicit parentheses for EL3==1 to fix
* 		      misra_c_2012_rule_12_1 violation.
* 7.7	sk   01/10/22 Add void argument to XTime_StartTTCTimer function to
* 		      fix misra_c_2012_rule_8_2 violation.
* 7.7	sk   01/10/22 Add braces for the if statement to make it a compound
* 		      statement and fix misra_c_2012_rule_15_6 violation.
*
* </pre>
*****************************************************************************/

/****************************  Include Files  ********************************/

#include "xil_io.h"
#include "xil_sleeptimer.h"
#include "xtime_l.h"

/****************************  Constant Definitions  *************************/


/* Function definitions are applicable only when TTC3 is present*/
#if defined (SLEEP_TIMER_BASEADDR)
/****************************************************************************/
/**
*
* This is a helper function used by sleep/usleep APIs to
* have delay in sec/usec
*
* @param            delay - delay time in seconds/micro seconds
*
* @param            frequency - Number of counts per second/micro second
*
* @return           none
*
* @note             none
*
*****************************************************************************/
void Xil_SleepTTCCommon(u32 delay, u64 frequency)
{
	u64 tEnd = 0U;
	u64 tCur = 0U;
	XCntrVal TimeHighVal = 0U;
	XCntrVal TimeLowVal1 = 0U;
	XCntrVal TimeLowVal2 = 0U;

	TimeLowVal1 = XSleep_ReadCounterVal(SLEEP_TIMER_BASEADDR +
			XSLEEP_TIMER_TTC_COUNT_VALUE_OFFSET);
	tEnd = (u64)TimeLowVal1 + ((u64)(delay) * frequency);
	do
	{
		TimeLowVal2 = XSleep_ReadCounterVal(SLEEP_TIMER_BASEADDR +
				                  XSLEEP_TIMER_TTC_COUNT_VALUE_OFFSET);
		if (TimeLowVal2 < TimeLowVal1) {
			TimeHighVal++;
		}
		TimeLowVal1 = TimeLowVal2;
		tCur = (((u64) TimeHighVal) << XSLEEP_TIMER_REG_SHIFT) |
								(u64)TimeLowVal2;
	}while (tCur < tEnd);
}


/*****************************************************************************/
/**
*
* This API starts the Triple Timer Counter
*
* @param            none
*
* @return           none
*
* @note             none
*
*****************************************************************************/
void XTime_StartTTCTimer(void)
{
	u32 TimerPrescalar;
	u32 TimerCntrl;

#if (defined (__aarch64__) && (EL3==1)) || (defined (ARMR5) && (PROCESSOR_ACCESS_VALUE & IOU_SLCR_TZ_MASK)) || defined (ARMA53_32)
	u32 LpdRst;

#if defined (versal)
	u32 RstAddr = CRL_TTC_RST;
	u32 RstMask = CRL_TTC_BASE_RST_MASK << XSLEEP_TTC_INSTANCE;
#else
	u32 RstAddr = RST_LPD_IOU2;
	u32 RstMask = RST_LPD_IOU2_TTC_BASE_RESET_MASK << XSLEEP_TTC_INSTANCE;
#endif
	/* check if the timer is reset */
    LpdRst = XSleep_ReadCounterVal(RstAddr);
    if ((LpdRst & RstMask) != 0U ) {
    	LpdRst = LpdRst & (~RstMask);
    	Xil_Out32(RstAddr, LpdRst);
	} else {
#endif
		TimerCntrl = XSleep_ReadCounterVal(SLEEP_TIMER_BASEADDR +
					XSLEEP_TIMER_TTC_CNT_CNTRL_OFFSET);
		/* check if Timer is disabled */
		if ((TimerCntrl & XSLEEP_TIMER_TTC_CNT_CNTRL_DIS_MASK) == 0U) {
		    TimerPrescalar = XSleep_ReadCounterVal(SLEEP_TIMER_BASEADDR +
					       XSLEEP_TIMER_TTC_CLK_CNTRL_OFFSET);
		/* check if Timer is configured with proper functionalty for sleep */
		   if ((TimerPrescalar & XSLEEP_TIMER_TTC_CLK_CNTRL_PS_EN_MASK) == 0U) {
						return;
		   }
		}
#if (defined (__aarch64__) && (EL3==1)) || (defined (ARMR5) && (PROCESSOR_ACCESS_VALUE & IOU_SLCR_TZ_MASK))  || defined (ARMA53_32)
	}
#endif
	/* Disable the timer to configure */
	TimerCntrl = XSleep_ReadCounterVal(SLEEP_TIMER_BASEADDR +
					XSLEEP_TIMER_TTC_CNT_CNTRL_OFFSET);
	TimerCntrl = TimerCntrl | XSLEEP_TIMER_TTC_CNT_CNTRL_DIS_MASK;
	Xil_Out32(SLEEP_TIMER_BASEADDR + XSLEEP_TIMER_TTC_CNT_CNTRL_OFFSET,
			                 TimerCntrl);
	/* Disable the prescalar */
	TimerPrescalar = XSleep_ReadCounterVal(SLEEP_TIMER_BASEADDR +
			XSLEEP_TIMER_TTC_CLK_CNTRL_OFFSET);
	TimerPrescalar = TimerPrescalar & (~XSLEEP_TIMER_TTC_CLK_CNTRL_PS_EN_MASK);
	Xil_Out32(SLEEP_TIMER_BASEADDR + XSLEEP_TIMER_TTC_CLK_CNTRL_OFFSET,
								TimerPrescalar);
	/* Enable the Timer */
	TimerCntrl = TimerCntrl & (~XSLEEP_TIMER_TTC_CNT_CNTRL_DIS_MASK);
	Xil_Out32(SLEEP_TIMER_BASEADDR + XSLEEP_TIMER_TTC_CNT_CNTRL_OFFSET,
								TimerCntrl);
}
#endif
