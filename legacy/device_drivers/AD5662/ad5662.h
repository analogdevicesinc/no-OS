/***************************************************************************//**
*   @file   ad5662.h
*   @brief  Header file of ad5662 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ad5662_H_
#define _ad5662_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define ad5662_SLAVE_ID         2

/******************************************************************************/
/* AD5662 Power Modes                                                         */
/******************************************************************************/
#define AD5662_LOAD             0x0  /* Load and update Normal Operation.*/
#define AD5662_PWRDWN_1K        0x1  /* Power-down: 1kOhm to GND. */
#define AD5662_PWRDWN_100K      0x2  /* Power-down: 100kOhm to GND. */
#define AD5662_PWRDWN_TRISTATE  0x3  /* Power-down: Three-state. */

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad5662_Init(void);

/* Writes 24 bits to device through SPI. */
char ad5662_Write24Bits(unsigned long data);

/* Sets the output DAC voltage to a desired value. */
void ad5662_SetVoltage(float vOut, float vRef);

#endif /* _ad5662_H_ */
