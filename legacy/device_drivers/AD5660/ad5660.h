/***************************************************************************//**
*   @file   ad5660.h
*   @brief  Header file of ad5660 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ad5660_H_
#define _ad5660_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
/* Supported devices */
typedef enum {
        ID_AD5620_2_3,
        ID_AD5620_1,
        ID_AD5640_2_3,
        ID_AD5640_1,
        ID_AD5660_2_3,
        ID_AD5660_1,
} AD56x0_type;

#define ad5660_SLAVE_ID         2

/******************************************************************************/
/* ad5660 Power Modes                                                         */
/******************************************************************************/
#define ad5660_LOAD             0x0  /* Load and update Normal Operation.*/
#define ad5660_PWRDWN_1K        0x1  /* Power-down: 1kOhm to GND. */
#define ad5660_PWRDWN_100K      0x2  /* Power-down: 100kOhm to GND. */
#define ad5660_PWRDWN_TRISTATE  0x3  /* Power-down: Three-state. */

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad5660_Init(AD56x0_type device);

/* Writes 12/14/16 bits of data to input register through SPI. */
char ad5660_WriteRegister(unsigned long data);

/* Sets the output DAC voltage to a desired value. */
void ad5660_SetVoltage(float vOut);

#endif /* _ad5660_H_ */
