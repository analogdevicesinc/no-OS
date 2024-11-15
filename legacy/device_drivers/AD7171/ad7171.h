/***************************************************************************//**
*   @file   ad7171.h
*   @brief  Header file of ad7171 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ad7171_H_
#define _ad7171_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define ad7171_SLAVE_ID     1

/* Supported devices */
typedef enum {
    ID_AD7170,
    ID_AD7171
} AD7171_type;

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad7171_Init(AD7171_type device);

/* Receives 24 bits from SPI and transmits 0x0. */
unsigned long ad7171_Read24Bits(unsigned long ctrl);

/* Calculates the input voltage and the pattern of the conversion as
parameter.*/
float ad7171_GetVoltage(unsigned long   ctrl,
                        unsigned char*  pattern,
                        float vRef);

/*  Returns the ADC code and the pattern of the conversion as parameter. */
unsigned short ad7171_GetAdcCode(unsigned long ctrl, unsigned char* pattern);

#endif /* _ad7171_H_ */
