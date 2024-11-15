/***************************************************************************//**
 *   @file   TIME.c
 *   @brief  Implementation of TIME Driver for PIC32MX320F128H Processor.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xc.h>
#include "TIME.h"

/******************************************************************************/
/************************ Variables Declarations ******************************/
/******************************************************************************/
static unsigned short overflowCounter = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Interrupt service routine.
 *
 * @return None.
*******************************************************************************/
void TIME_ISR(void)
{
    overflowCounter++;
}

/***************************************************************************//**
 * @brief Initializes the timer used in this driver.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: -1 - if initialization was unsuccessful;
 *                            0 - if initialization was successful.
*******************************************************************************/
char TIME_Init(void)
{
    return 0;
}

/***************************************************************************//**
 * @brief The timer begins to count in steps of microseconds(us) until the user
 *        calls a stop measurement function.
 *
 * @return None.
*******************************************************************************/
void TIME_StartMeasure(void)
{
   _CP0_SET_COUNT(0);
}

/***************************************************************************//**
 * @brief Stops the measurement process when the functions is called.
 *
 * @return Time(in microseconds) elapsed since the measurement began.
*******************************************************************************/
unsigned long TIME_StopMeasure(void)
{
   return (_CP0_GET_COUNT() / 40);
}

/***************************************************************************//**
 * @brief Creates a delay of microseconds.
 *
 * @return None.
*******************************************************************************/
void TIME_DelayUs(unsigned short usUnits)
{
    unsigned long ticks = 0;

    _CP0_SET_COUNT(0);
    while(ticks < TICKS_FOR_1US * usUnits)
    {
        ticks = _CP0_GET_COUNT();
    }
}

/***************************************************************************//**
 * @brief Creates a delay of milliseconds.
 *
 * @return None.
*******************************************************************************/
void TIME_DelayMs(unsigned short msUnits)
{
    unsigned long ticks = 0;

    _CP0_SET_COUNT(0);
    while(ticks < TICKS_FOR_1MS * msUnits)
    {
        ticks = _CP0_GET_COUNT();
    }
}
