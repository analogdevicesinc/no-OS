/***************************************************************************//**
 *   @file   TIME.c
 *   @brief  Implementation of TIME Driver for PIC32MX320F128H Processor.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*******************************************************************************/

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
