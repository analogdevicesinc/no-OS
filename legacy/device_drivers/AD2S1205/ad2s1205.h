/***************************************************************************//**
*   @file   ad2s1205.h
*   @brief  Header file of ad2s1205 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AD2S1205_H_
#define _AD2S1205_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/* PARALEL PORT                                                               */
/******************************************************************************/
#define PAR_RDVEL       (1 << 0)
#define PAR_SAMPLE      (1 << 1)
#define PAR_CS          (1 << 5)
#define PAR_DATA        (1 << 6)
#define PAR_RD          (1 << 26)

#define PAR_DATA_MASK   (0x00000FFF << 6)

/******************************************************************************/
/* Timing constants                                                           */
/******************************************************************************/
#define F_CLKIN         8192 //KHz
#define T2              ((int)6000000/F_CLKIN + 20)/2 //(6/fCLKIN)+20 [ns]
#define T3              18  //[ns]
#define T4              5   //[ns]
#define T5              7   //[ns]
#define T6              30  //[ns]
#define T7              18  //[ns]

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/

/* Initializes the parallel communication with the device. */
char AD2S1205_Init(void);

/* Reads the position and velocity information through parallel port. */
void AD2S1205_Pos_Vel(unsigned short* position, short* velocity);

#endif /* AD2S1205_H_ */
