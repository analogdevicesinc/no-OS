/***************************************************************************//**
 *   @file   TIME.c
 *   @brief  Implementation of delay functions.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "TIME.h"
#include "xparameters.h"

/******************************************************************************/
/******************** Macros and Constants Definitions ************************/
/******************************************************************************/
#define TIMER_BASEADDR  XPAR_AXI_TIMER_0_BASEADDR

/***************************************************************************//**
 * @brief Creates a delay of nanoseconds.
 *
 * @return None.
*******************************************************************************/
void TIME_DelayNs(unsigned short nsUnits)
{
    TIMER0_WAIT(TIMER_BASEADDR, nsUnits);
}

/***************************************************************************//**
 * @brief Creates a delay of microseconds.
 *
 * @return None.
*******************************************************************************/
void TIME_DelayUs(unsigned short usUnits)
{
    TIMER0_WAIT(TIMER_BASEADDR, (1000 * usUnits));
}

/***************************************************************************//**
 * @brief Creates a delay of milliseconds.
 *
 * @return None.
*******************************************************************************/
void TIME_DelayMs(unsigned short msUnits)
{
    TIMER0_WAIT(TIMER_BASEADDR, (1000000L * msUnits));
}
