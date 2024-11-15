/***************************************************************************//**
*   @file   AD5425.h
*   @brief  Header file of AD5425 Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
********************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _AD5425_H_
#define _AD5425_H_

/******************************************************************************/
/*********************************** Include **********************************/
/******************************************************************************/
#include "Communication.h"

/* SPI slave device ID */
#define AD5425_SLAVE_ID    		1

/* AD5415 GPIO */
#define AD5425_LDAC_OUT         GPIO1_PIN_OUT
#define AD5425_LDAC_LOW         GPIO1_LOW
#define AD5425_LDAC_HIGH        GPIO1_HIGH

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/

int AD5425_Init(void);
void AD5425_SetRegister(unsigned char data);

#endif
