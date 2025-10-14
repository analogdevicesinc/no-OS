/******************************************************************************
* Copyright (c) 2014 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpm_counter.c
*
* This file contains APIs for configuring and controlling the Cortex-R5
* Performance Monitor Events. For more information about the event counters,
* see xpm_counter.h.
* The file contains APIs to setup an event, return the event counter value,
* disable event(s), enable events, reset event counters.
* It also provides a helper function: Xpm_SleepPerfCounter that is used to
* implement sleep routines in non-OS environment.
* It also contains two APIs which are being deprecated. Users are advised
* not to use them.
* On usage of these APIs, please refer to xpm_conter_example.c file which can
* be found at standalone example folder.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 5.00  pkp  02/10/14 Initial version
* 6.2   mus  01/27/17 Updated to support IAR compiler
* 7.1   aru  04/15/19 Updated the events correctly
* 7.2   mus  01/29/20 Added helper function Xpm_SleepPerfCounter for sleep
*                     routines. It would be consumed by sleep routines for
*                     delay generation through CortexR5 PMU cycle counter,if
*                     TTC2 as well as TTC3 is not present in HW design. User
*                     can add compiler flag "DONT_USE_PMU_FOR_SLEEP_ROUTINES"
*                     in BSP compiler flags to avoid using PMU cycle counter
*                     for sleep routines.
* 7.2   asa  03/18/20 Add implementation for new APIs that simplifies the
*                     existing event handling mechanism.
*                     Older APIs are being deprecated.
* 7.6   mus  09/16/21 PmcrEventCfg32 is declared with deprecate attribute and
*                     being used in one of the deprecated function,
*                     that is always resulting into complialation
*                     warning. Removed deprecate attribute from PmcrEventCfg32
*                     to fix un-necessary warning CR#1110990.
* 7.7	sk   01/10/22 Update values from signed to unsigned to fix
* 		      misra_c_2012_rule_10_4 violation.
* 7.7	sk   01/10/22 Typecast to fix wider essential type misra_c_2012_rule_10_7
* 		      violation.
* 7.7	sk   03/02/22 Remove continue statement from else condition to fix
* 		      NO_EFFECT violation.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xpm_counter.h"
#ifndef XPAR_XILTIMER_ENABLED
#include "xil_sleeptimer.h"
#include "xtime_l.h"
#endif

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/
typedef const u32 PmcrEventCfg32[XPM_CTRCOUNT];

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions *****************************/


/******************************************************************************/

/****************************************************************************/
/**
*
* @brief    This function disables the Cortex R5 event counters.
*
* @return	None.
*
*****************************************************************************/
void Xpm_DisableEventCounters(void)
{
    u32 RegVal;
    /* Disable the event counters */
#ifdef __GNUC__
    RegVal = mfcp(XREG_CP15_COUNT_ENABLE_CLR);
#elif defined (__ICCARM__)
    mfcp(XREG_CP15_COUNT_ENABLE_CLR, RegVal);
#else
    { register u32 C15Reg __asm(XREG_CP15_COUNT_ENABLE_CLR);
      RegVal = C15Reg; }
#endif
    RegVal |= XPM_EVENT_CNTRS_MASK;
    mtcp(XREG_CP15_COUNT_ENABLE_CLR, RegVal);
}

/****************************************************************************/
/**
*
* @brief    This function enables the Cortex R5 event counters.
*
* @return	None.
*
*****************************************************************************/
void Xpm_EnableEventCounters(void)
{
    u32 RegVal;
    /* Enable the event counters */
#ifdef __GNUC__
    RegVal = mfcp(XREG_CP15_COUNT_ENABLE_SET);
#elif defined (__ICCARM__)
    mfcp(XREG_CP15_COUNT_ENABLE_SET, RegVal);
#else
    { register u32 C15Reg __asm(XREG_CP15_COUNT_ENABLE_SET);
      RegVal = C15Reg; }
#endif
    RegVal |= XPM_EVENT_CNTRS_MASK;
	mtcp(XREG_CP15_COUNT_ENABLE_SET, RegVal);
}

/****************************************************************************/
/**
*
* @brief    This function resets the Cortex R5 event counters.
*
* @return	None.
*
*****************************************************************************/
void Xpm_ResetEventCounters(void)
{
    u32 Reg;

#ifdef __GNUC__
    Reg = mfcp(XREG_CP15_PERF_MONITOR_CTRL);
#elif defined (__ICCARM__)
    mfcp(XREG_CP15_PERF_MONITOR_CTRL, Reg);
#else
	{ register u32 C15Reg __asm(XREG_CP15_PERF_MONITOR_CTRL);
	  Reg = C15Reg; }
#endif
    Reg |= (1U << 2U); /* reset event counters */
    mtcp(XREG_CP15_PERF_MONITOR_CTRL, Reg);
}

/*****************************************************************************/
/**
 *
 * Disables the requested event counter.
 *
 *
 * @param	EventCntrId: Event Counter ID. The counter ID is the same that
 *          was earlier returned through a call to Xpm_SetUpAnEvent.
 *          Cortex-R5 supports only 3 counters. The valid values are 0, 1,
 *          or 2.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *		- XST_FAILURE if the passed Counter ID is invalid
 *        (i.e. greater than 2).
 *
 ******************************************************************************/
u32 Xpm_DisableEvent(u32 EventCntrId)
{
    u32 Counters;
    u32 CntrMask = 0x1U;

    if (EventCntrId > XPM_MAX_EVENTHANDLER_ID) {
        xil_printf("Invalid EventHandlerID\r\n");
        return XST_FAILURE;
    } else {
        CntrMask = CntrMask << EventCntrId;
#ifdef __GNUC__
        Counters = mfcp(XREG_CP15_COUNT_ENABLE_CLR);
#elif defined (__ICCARM__)
        mfcp(XREG_CP15_COUNT_ENABLE_CLR, Counters);
#else
        { register u32 C15Reg __asm(XREG_CP15_COUNT_ENABLE_CLR);
		Counters = C15Reg; }
#endif
		Counters &= ~CntrMask;
        mtcp(XREG_CP15_COUNT_ENABLE_CLR, Counters);
        return XST_SUCCESS;
    }
}

/*****************************************************************************/
/**
 *
 * Sets up one of the event counters to count events based on the Event ID
 * passed. For supported Event IDs please refer xpm_counter.h.
 * Upon invoked, the API searches for an available counter. After finding
 * one, it sets up the counter to count events for the requested event.
 *
 *
 * @param	EventID: For valid values, please refer xpm_counter.h.
 *
 * @return
 *		- Counter Number if successful. For Cortex-R5, valid return values are
 *        0, 1, or 2.
 *		- XPM_NO_COUNTERS_AVAILABLE (0xFF) if all counters are being used
 *
 ******************************************************************************/
u32 Xpm_SetUpAnEvent(u32 EventID)
{
    u32 Counters;
    u32 OriginalCounters;
    u32 Index;

#ifdef __GNUC__
    OriginalCounters = mfcp(XREG_CP15_COUNT_ENABLE_SET);
#elif defined (__ICCARM__)
    mfcp(XREG_CP15_COUNT_ENABLE_SET, OriginalCounters);
#else
    { register u32 C15Reg __asm(XREG_CP15_COUNT_ENABLE_SET);
	OriginalCounters = C15Reg; }
#endif
    OriginalCounters &= XPM_EVENT_CNTRS_BIT_MASK;
    Counters = OriginalCounters;
    if (Counters == XPM_ALL_EVENT_CNTRS_IN_USE) {
        xil_printf("No counters available\r\n");
        return XPM_NO_COUNTERS_AVAILABLE;
	} else {
        for(Index = 0U; Index < XPM_CTRCOUNT; Index++) {
	     if ((Counters & 0x1U) == 0x0U) {
			    break;
            } else {
                Counters = Counters >> 0x1U;
            }
        }
    }

    /* Select event counter */
    mtcp(XREG_CP15_EVENT_CNTR_SEL, Index);
    /* Set the event */
    mtcp(XREG_CP15_EVENT_TYPE_SEL, EventID);
    /* Enable event counter */
    mtcp(XREG_CP15_COUNT_ENABLE_SET, OriginalCounters | ((u32)1U << Index));
    return Index;
}

/*****************************************************************************/
/**
 *
 * Reads the counter value for the requested counter ID. This is used to read
 * the number of events that has been counted for the requsted event ID.
 * This can only be called after a call to Xpm_SetUpAnEvent.
 *
 *
 * @param	EventCntrId: The counter ID is the same that was earlier
 *          returned through a call to Xpm_SetUpAnEvent.
 *          Cortex-R5 supports only 3 counters. The valid values are 0, 1,
 *          or 2.
 * @param	CntVal: Pointer to a 32 bit unsigned int type. This is used to return
 *          the event counter value.
 *
 * @return
 *		- XST_SUCCESS if successful.
 *		- XST_FAILURE if the passed Counter ID is invalid
 *        (i.e. greater than 2).
 *
 ******************************************************************************/
u32 Xpm_GetEventCounter(u32 EventCntrId, u32 *CntVal)
{
    if (EventCntrId > XPM_MAX_EVENTHANDLER_ID) {
        xil_printf("Invalid Event Handler ID\r\n");
        return XST_FAILURE;
    } else {
        mtcp(XREG_CP15_EVENT_CNTR_SEL, EventCntrId);
#ifdef __GNUC__
        *CntVal = mfcp(XREG_CP15_PERF_MONITOR_COUNT);
#elif defined (__ICCARM__)
        mfcp(XREG_CP15_PERF_MONITOR_COUNT, (*CntVal));
#else
        { register u32 C15Reg __asm(XREG_CP15_PERF_MONITOR_COUNT);
	    *CntVal = C15Reg; }
#endif
        return XST_SUCCESS;
    }
}

/****************************************************************************/
/**
*
* @brief    This is helper function used by sleep/usleep APIs to generate
*           delay in sec/usec
*
* @param    delay - delay time in sec/usec
* @param    frequency - Number of countes in second/micro second
*
* @return       None.
*
*****************************************************************************/
#ifndef XPAR_XILTIMER_ENABLED
void Xpm_SleepPerfCounter(u32 delay, u64 frequency)
{
    u64 tEnd = 0U;
    u64 tCur = 0U;
    XCntrVal TimeHighVal = 0U;
    XCntrVal TimeLowVal1 = 0U;
    XCntrVal TimeLowVal2 = 0U;

#if defined (__GNUC__)
    TimeLowVal1 = Xpm_ReadCycleCounterVal();
#elif defined (__ICCARM__)
    Xpm_ReadCycleCounterVal(TimeLowVal1);
#endif
    tEnd = (u64)TimeLowVal1 + ((u64)(delay) * frequency);
    do {
#if defined (__GNUC__)
        TimeLowVal2 = Xpm_ReadCycleCounterVal();
#elif defined (__ICCARM__)
        Xpm_ReadCycleCounterVal(TimeLowVal2);
#endif
        if (TimeLowVal2 < TimeLowVal1) {
		    TimeHighVal++;
        }
        TimeLowVal1 = TimeLowVal2;
        tCur = (((u64) TimeHighVal) << XSLEEP_TIMER_REG_SHIFT) |
		                                           (u64)TimeLowVal2;
        }while (tCur < tEnd);
}
#endif
/****************************************************************************/
/**
*
* @brief    This function configures the Cortex R5 event counters controller,
*           with the event codes, in a configuration selected by the user and
*           enables the counters.
*
* @param	PmcrCfg: Configuration value based on which the event counters
*		    are configured.XPM_CNTRCFG* values defined in xpm_counter.h can
*		    be utilized for setting configuration
*
* @return	None.
*
*****************************************************************************/
void Xpm_SetEvents(s32 PmcrCfg)
{
	u32 Counter;
	static PmcrEventCfg32 PmcrEvents[] = {

		{
			XPM_EVENT_SOFTINCR,
			XPM_EVENT_INSTRCACHEMISS,
			XPM_EVENT_DATACACHEMISS
		},
		{
			XPM_EVENT_DATACACHEACCESS,
			XPM_EVENT_DATAREAD,
			XPM_EVENT_DATAWRITE
		},
		{
			XPM_EVENT_INSTR,
			XPM_EVENT_DUALINSTR,
			XPM_EVENT_EXCEPTION
		},
		{
			XPM_EVENT_EXCEPTIONRET,
			XPM_EVENT_CHANGETOCONTEXID,
			XPM_EVENT_SWCHANGE
		},
		{
			XPM_EVENT_IMMEDIATEINSTR,
			XPM_EVENT_PROCEDURERET,
			XPM_EVENT_UNALIGNACCESS
		},
		{
			XPM_EVENT_BRANCHMISPREDICT,
			XPM_EVENT_CLOCKCYCLES,
			XPM_EVENT_BRANCHPREDICT
		},
		{
			XPM_EVENT_INSTRSTALL,
			XPM_EVENT_DATASTALL,
			XPM_EVENT_DATACACHEWRITE
		},
		{
			XPM_EVENT_EXTERNALMEMREQ,
			XPM_EVENT_LSUSTALL,
			XPM_EVENT_FORCEDRAINSTORE
		},
		{
			XPM_EVENT_INSTRTAGPARITY,
			XPM_EVENT_INSTRDATAPARITY,
			XPM_EVENT_DATATAGPARITY
		},
		{
			XPM_EVENT_DATADATAPARITY,
			XPM_EVENT_TCMERRORPREFETCH,
			XPM_EVENT_TCMERRORSTORE
		},
		{
			XPM_EVENT_INSTRCACHEACCESS,
			XPM_EVENT_DUALISSUEA,
			XPM_EVENT_DUALISSUEB
		},
		{
			XPM_EVENT_DUALISSUEOTHER,
			XPM_EVENT_FPA,
			XPM_EVENT_DATACACHEDATAERROR
		},
		{
			XPM_EVENT_DATACACHETAGERROR,
			XPM_EVENT_PROCESSORLIVELOCK,
			XPM_EVENT_ATCMMULTIBITERROR
		},
		{
			XPM_EVENT_B0TCMMULTIBITERROR,
			XPM_EVENT_B1TCMMULTIBITERROR,
			XPM_EVENT_ATCMSINGLEBITERROR
		},
		{
			XPM_EVENT_B0TCMSINGLEBITERROR,
			XPM_EVENT_B1TCMSINGLEBITERROR,
			XPM_EVENT_TCMERRORLSU
		},
		{
			XPM_EVENT_TCMERRORPFU,
			XPM_EVENT_TCMFATALERRORAXI,
			XPM_EVENT_TCMERRORAXI
		},

	};
	const u32 *ptr = PmcrEvents[PmcrCfg];

	Xpm_DisableEventCounters();

	for(Counter = 0U; Counter < XPM_CTRCOUNT; Counter++) {

		/* Select event counter */
		mtcp(XREG_CP15_EVENT_CNTR_SEL, Counter);

		/* Set the event */
		mtcp(XREG_CP15_EVENT_TYPE_SEL, ptr[Counter]);
	}

	Xpm_ResetEventCounters();
	Xpm_EnableEventCounters();
}

/****************************************************************************/
/**
*
* @brief    This function disables the event counters and returns the counter
*           values.
*
* @param	PmCtrValue: Pointer to an array of type u32 PmCtrValue[6].
*		    It is an output parameter which is used to return the PM
*		    counter values.
*
* @return	None.
*
*****************************************************************************/
void Xpm_GetEventCounters(u32 *PmCtrValue)
{
	u32 Counter;

	Xpm_DisableEventCounters();

	for(Counter = 0U; Counter < XPM_CTRCOUNT; Counter++) {

		mtcp(XREG_CP15_EVENT_CNTR_SEL, Counter);
#ifdef __GNUC__
		PmCtrValue[Counter] = mfcp(XREG_CP15_PERF_MONITOR_COUNT);
#elif defined (__ICCARM__)
        mfcp(XREG_CP15_PERF_MONITOR_COUNT, PmCtrValue[Counter]);
#else
		{ register u32 Cp15Reg __asm(XREG_CP15_PERF_MONITOR_COUNT);
		  PmCtrValue[Counter] = Cp15Reg; }
#endif
	}
}


