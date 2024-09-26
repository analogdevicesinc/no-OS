/***************************************************************************//**
 *   @file   TIME.h
 *   @brief  Header file of TIME Driver for PIC32MX320F128H Processor.
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
#ifndef __TIME_H__
#define __TIME_H__

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/
#define SYSTEM_CLOCK        80000000 		   /*!< System clock frequency is 80MHz */
#define CORE_TIMER_CLOCK    SYSTEM_CLOCK / 2.0 /*!< Core timer runs at 40 MHz */
#define MICROS_IN_A_SECOND  1000000			   /*!< Number of microseconds in a second */
#define MILLIS_IN_A_SECOND  1000			   /*!< Number of millisecons in a second */
static const unsigned long  TICKS_FOR_1US = CORE_TIMER_CLOCK /
                                            MICROS_IN_A_SECOND;
static const unsigned long  TICKS_FOR_1MS = CORE_TIMER_CLOCK /
                                            MILLIS_IN_A_SECOND;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*!< Initializes the timer used in this driver. */
char TIME_Init(void);

/*!< The timer begins to count in steps of microseconds(us) until the user calls
    a stop measurement function. */
void TIME_StartMeasure(void);

/*!< Stops the measurement process when the functions is called. */
unsigned long TIME_StopMeasure(void);

/*!< Creates a delay of microseconds. */
void TIME_DelayUs(unsigned short usUnits);

/*!< Creates a delay of milliseconds. */
void TIME_DelayMs(unsigned short msUnits);

#endif /* __TIME_H__ */
