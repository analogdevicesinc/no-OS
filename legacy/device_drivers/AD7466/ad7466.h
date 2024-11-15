/***************************************************************************//**
*   @file   ad7466.h
*   @brief  Header file of ad7466 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AD7466_H_
#define _AD7466_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define AD7466_SLAVE_ID     1

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char AD7466_Init(void);

/* Receives 16 bits from SPI. */
unsigned short AD7466_Read16Bits(void);

#endif /* _AD7466_H_ */
