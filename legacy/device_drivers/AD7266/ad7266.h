/***************************************************************************//**
*   @file   ad7266.h
*   @brief  Header file of ad7266 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ad7266_H_
#define _ad7266_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define ad7266_SLAVE_ID     1

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad7266_Init(void);

/* Receives 32 bits from SPI and transmits 0x0. */
unsigned long ad7266_Read32Bits(unsigned long ctrl);

#endif /* _ad7266_H_ */
