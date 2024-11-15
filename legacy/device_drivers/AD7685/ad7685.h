/***************************************************************************//**
*   @file   ad7685.h
*   @brief  Header file of ad7685 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ad7685_H_
#define _ad7685_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define ad7685_SLAVE_ID         1

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad7685_Init(void);

/* @brief Makes a conversion for a number of devices daisy-chained (only works
for Chain Mode, no busy indicator). */
char ad7685_GetConversion(char partsNo, unsigned short *dataDac);

/* Converts a 16-bit raw sample to volts. */
float ad7685_ConvertToVolts(unsigned short rawSample, float vRef);

#endif /* _ad7685_H_ */
