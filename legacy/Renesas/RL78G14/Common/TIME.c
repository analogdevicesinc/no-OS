/***************************************************************************//**
 *   @file   TIME.c
 *   @brief  Implementation of TIME Driver for RENESAS RL78G14 Processor.
 *   @author Dan Nechita
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "RDKRL78G14.h"    // RDKRL78G14 definitions.
#include "TIME.h"          // TIME definitions.

/*****************************************************************************/
/************************** Macros Definitions *******************************/
/*****************************************************************************/
#define TIMER_CNT_INTERVAL      65536

/*****************************************************************************/
/************************ Variables Declarations *****************************/
/*****************************************************************************/
unsigned short overflowCounter = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief INTTM00 interrupt service routine.
 *
 * @return None.
*******************************************************************************/
#pragma vector = INTTM00_vect
__interrupt static void Timer0InterruptHandler(void)
{
    overflowCounter++;
}

/***************************************************************************//**
 * @brief Initializes the timer used in this driver.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char TIME_Init(void)
{
    PER0   |= 0x0001;   // Enable Supply for Unit 0 that contains timers 0..7.
    TT0    |= 0x0001;   // Stop channel 0(Timer 0).
    TPS0    = 0x0005;   // CK00 = FCLK / 32.
    TMR00  |= 0x0000;   // CK00 input clock for channel 0.
    TMMK00  = 1;        // Disable INTTM00 interrupt.
    TMIF00  = 0;        // Clear INTM00 interrupt flag.
    /* Set INTTM00 low priority */
    TMPR100 = 1;
    TMPR000 = 1;
    /* Configure channel 0 as a interval timer. */
    TMR00  |= 0x0000;   // Interval timer mode.
    TO0    &= ~0x0001;  // Timer 0 output value is "0".
    TOE0   &= ~0x0001;  // Disable output of timer 0.
    TMMK00  = 0;        // Enable INTTM00 interrupt.
    
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
    TDR00  = 0xFFFF;
    overflowCounter = 0;
    TS0   |= 0x0001;    // Start timer 0.
    TMIF00 = 0;         // Clear INTM00 interrupt flag.
}

/***************************************************************************//**
 * @brief Stops the measurement process when the functions is called.
 *
 * @return Time(in microseconds) elapsed since the measurement began.
*******************************************************************************/
unsigned long TIME_StopMeasure(void)
{
    unsigned long  elapsedUs = 0;
    unsigned short timerCnt  = 0;
    
    timerCnt  = TCR00;
    TT0      |= 0x0001;   // Stop Timer 0.
    TMIF00    = 0;        // Clear INTM00 interrupt flag.
    overflowCounter = 0;
    elapsedUs =  ((unsigned long)overflowCounter * TIMER_CNT_INTERVAL) + 
             (0xFFFF - timerCnt);
    
    return elapsedUs;
}

/***************************************************************************//**
 * @brief Creates a delay of microseconds.
 *
 * @return None.
*******************************************************************************/
void TIME_DelayUs(unsigned short usUnits)
{
    TDR00 = usUnits;
    TS0  |= 0x0001; // Start timer 0.
    while(!overflowCounter)
    {
        ;
    }
    overflowCounter = 0;
    TT0  |= 0x0001; // Stop Timer 0.
}

/***************************************************************************//**
 * @brief Creates a delay of milliseconds.
 *
 * @return None.
*******************************************************************************/
void TIME_DelayMs(unsigned short msUnits)
{
    TDR00 = 0x3E7;  // 0x3E7 = 999(decimal). Timer will count down to 0.
    TS0  |= 0x0001; // Start timer 0.
    while(overflowCounter < msUnits)
    {
        ;
    }
    overflowCounter = 0;
    TT0  |= 0x0001; // Stop Timer 0.   
}
