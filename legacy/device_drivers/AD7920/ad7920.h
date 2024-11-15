/***************************************************************************//**
*   @file   ad7920.h
*   @brief  Header file of AD7920 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AD7920_H_
#define _AD7920_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define AD7920_SLAVE_ID             1

/* GPIOs */
#define AD7920_SD_PIN_OUT           GPIO7_PIN_OUT
#define AD7920_SD_HIGH              GPIO7_HIGH
#define AD7920_SD_LOW               GPIO7_LOW

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char AD7920_Init(void);

/* Reads the output ADC code.. */
unsigned short ad7920_GetAdcCode(void);

/* Calculates the input voltage. */
float ad7920_GetVoltage(float vRef);

#endif /* _AD7920_H_ */
