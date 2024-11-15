/***************************************************************************//**
 *   @file   TIME.c
 *   @brief  Header for delay functions.
 *   @author acozma (andrei.cozma@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __TIME_H__
#define __TIME_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xil_io.h"

/*****************************************************************************/
/******************* Timer Registers Definitions *****************************/
/*****************************************************************************/
// Register address
#define TCSR0	0x00
#define TLR0	0x04
#define TCR0	0x08
#define TCSR1	0x10
#define TLR1	0x14
#define TCR1	0x18

/*****************************************************************************/
/******************* Timer Registers Bits ************************************/
/*****************************************************************************/
// control/status register bits
#define MDT		(1 << 0)
#define UDT		(1 << 1)
#define GENT	(1 << 2)
#define CAPT	(1 << 3)
#define ARHT	(1 << 4)
#define LOAD	(1 << 5)
#define EINT	(1 << 6)
#define ENT		(1 << 7)
#define TINT	(1 << 8)
#define PWMA	(1 << 9)
#define ENALL	(1 << 10)
#define CASC	(1 << 11)

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/
/*CPU cycle time in ns, cycle time = 1/CPU_CLOCK_RATE*/
#define CPU_CYCLE_TIME 10

#define TIMER0_WAIT(timerBaseAddr, ns) \
	Xil_Out32(timerBaseAddr + TLR0,  (ns < CPU_CYCLE_TIME ? 1 : ns/CPU_CYCLE_TIME)); \
	Xil_Out32(timerBaseAddr + TCSR0, (UDT | LOAD)); \
	Xil_Out32(timerBaseAddr + TCSR0, (UDT | ENT)); \
	while(Xil_In32(timerBaseAddr + TCR0)!=-1);

/*****************************************************************************/
/*************************** Functions Prototypes*****************************/
/*****************************************************************************/
/*! Creates a delay of nanoseconds. */
void TIME_DelayNs(unsigned short nsUnits);

/*! Creates a delay of microseconds. */
void TIME_DelayUs(unsigned short usUnits);

/*! Creates a delay of milliseconds. */
void TIME_DelayMs(unsigned short msUnits);

#endif /*__TIMER_H__*/
