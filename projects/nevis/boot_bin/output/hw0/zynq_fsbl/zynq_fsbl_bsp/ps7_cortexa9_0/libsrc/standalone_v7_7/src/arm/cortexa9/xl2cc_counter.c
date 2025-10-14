/******************************************************************************
* Copyright (c) 2011 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xl2cc_counter.c
*
* This file contains APIs for configuring and controlling the event counters
* in PL310 L2 cache controller. For more information about the event counters,
* see xl2cc_counter.h.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sdm  07/11/11 First release
* 3.07a asa  08/30/12 Updated for CR 675636 to provide the L2 Base Address
*		      inside the APIs
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include <stdint.h>
#include "xparameters_ps.h"
#include "xl2cc_counter.h"
#include "xl2cc.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

void XL2cc_EventCtrReset(void);

/******************************************************************************/

/****************************************************************************/
/**
*
* @brief	This function initializes the event counters in L2 Cache controller
*			with a set of event codes specified by the user.
*
* @param	Event0: Event code for counter 0.
* @param	Event1: Event code for counter 1.
*
* @return	None.
*
* @note		The definitions for event codes XL2CC_* can be found in
*			xl2cc_counter.h.
*
*****************************************************************************/
void XL2cc_EventCtrInit(s32 Event0, s32 Event1)
{

	/* Write event code into cnt1 cfg reg */
	*((volatile u32*)(XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNT1_CTRL_OFFSET)) = (((u32)Event1) << 2);

	/* Write event code into cnt0 cfg reg */
	*((volatile u32*)(XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNT0_CTRL_OFFSET)) = (((u32)Event0) << 2);

	/* Reset counters */
	XL2cc_EventCtrReset();
}


/****************************************************************************/
/**
*
* @brief	This function starts the event counters in L2 Cache controller.
*
* @return	None.
*
*****************************************************************************/
void XL2cc_EventCtrStart(void)
{
	u32 *LocalPtr;
	LocalPtr = (u32 *)(XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNTRL_OFFSET);
	XL2cc_EventCtrReset();

	/* Enable counter */
	/* *((volatile u32*)((void *)(XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNTRL_OFFSET))) = 1 */
	*LocalPtr = (u32)1;
}

/****************************************************************************/
/**
*
* @brief	This function disables the event counters in L2 Cache controller,
*			saves the counter values and resets the counters.
*
* @param	EveCtr0: Output parameter which is used to return the value
*			in event counter 0.
* @param	EveCtr1: Output parameter which is used to return the value
*			in event counter 1.
*
* @return	None.
*
*****************************************************************************/
void XL2cc_EventCtrStop(u32 *EveCtr0, u32 *EveCtr1)
{
	/* Disable counter */
	*((volatile u32*) (XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNTRL_OFFSET)) = 0U;

	/* Save counter values */
	*EveCtr1 = *((volatile u32*)(XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNT1_VAL_OFFSET));
	*EveCtr0 = *((volatile u32*)(XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNT0_VAL_OFFSET));

	XL2cc_EventCtrReset();
}

/****************************************************************************/
/**
*
* @brief	This function resets the event counters in L2 Cache controller.
*
* @return	None.
*
*****************************************************************************/
void XL2cc_EventCtrReset(void)
{
	*((volatile u32*)(XPS_L2CC_BASEADDR + XPS_L2CC_EVNT_CNTRL_OFFSET)) = 0x6U;
}
