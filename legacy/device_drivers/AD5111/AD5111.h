/**************************************************************************//**
*   @file   AD5111.h
*   @brief  Header file of AD5111 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _AD5111_H_
#define _AD5111_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/***************************************************************************/
/************************ Functions Declarations ***************************/
/***************************************************************************/

/* Increases the resistor value with one step. */
void incValue(void);

/* Decreases the resistor value with one step. */
void decValue(void);

/* Device enters in shutdown mode. */
void shutDown(void);

/* Device memorizes the current RDAC value in EEPROM. */
void saveMem(void);

/* Initializes the communication with the device. */
unsigned char AD5111_Init(void);

#endif	// _AD5111_H_
