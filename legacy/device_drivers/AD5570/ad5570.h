/***************************************************************************//**
*   @file   AD5570.h
*   @brief  Header file of AD5570 Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
********************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _AD5570_H_
#define _AD5570_H_

/******************************************************************************/
/*********************************** Include **********************************/
/******************************************************************************/
#include "Communication.h"

/* SPI slave device ID */
#define AD5570_SLAVE_ID         1

/* AD5570 GPIO */
#define AD5570_LDAC_OUT         GPIO0_PIN_OUT
#define AD5570_LDAC_LOW         GPIO0_LOW
#define AD5570_LDAC_HIGH        GPIO0_HIGH

#define AD5570_CLR_OUT          GPIO1_PIN_OUT
#define AD5570_CLR_LOW          GPIO1_LOW
#define AD5570_CLR_HIGH         GPIO1_HIGH

#define AD5570_PD_OUT           GPIO2_PIN_OUT
#define AD5570_PD_LOW           GPIO2_LOW
#define AD5570_PD_HIGH          GPIO2_HIGH

/******************************************************************************/
/************************** Functions Declarations ****************************/
/******************************************************************************/

int AD5570_Init(void);
void AD5570_SetRegister(unsigned short data);

#endif
