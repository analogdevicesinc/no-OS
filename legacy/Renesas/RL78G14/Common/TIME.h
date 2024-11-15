/***************************************************************************//**
 *   @file   TIME.h
 *   @brief  Header file of TIME Driver for RENESAS RL78G14 Processor.
 *   @author Dan Nechita
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __TIME_H__
#define __TIME_H__

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes the timer used in this driver. */
char TIME_Init(void);

/*! The timer begins to count in steps of microseconds(us) until the user calls 
    a stop measurement function. */
void TIME_StartMeasure(void);

/*! Stops the measurement process when the functions is called. */
unsigned long TIME_StopMeasure(void);

/*! Creates a delay of microseconds. */
void TIME_DelayUs(unsigned short usUnits);

/*! Creates a delay of milliseconds. */
void TIME_DelayMs(unsigned short msUnits);

#endif /* __TIME_H__ */
