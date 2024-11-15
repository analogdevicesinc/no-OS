/***************************************************************************//**
 *   @file   TIME.h
 *   @brief  Header file of TIME Driver for PIC32MX320F128H Processor.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
