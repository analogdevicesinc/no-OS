/***************************************************************************//**
*   @file   ad7887.h
*   @brief  Header file of ad7887 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _AD7887_H_
#define _AD7887_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define AD7887_SLAVE_ID         1

/******************************************************************************/
/* AD7887 control register bits                                               */
/******************************************************************************/
#define ad7887_CTRL_DONTC		(1 << 7)     /* Don't care bit */
#define ad7887_CTRL_ZERO_H		(1 << 6)     /* Zero bit */
#define ad7887_CTRL_REF  		(1 << 5)     /* Reference bit */
#define ad7887_CTRL_SIN_DUAL	(1 << 4)     /* Single/Dual bit */
#define ad7887_CTRL_CH  		(1 << 3)     /* Channel bit */
#define ad7887_CTRL_ZERO_L 		(1 << 2)     /* Zero bit */
#define ad7887_CTRL_PM1  		(1 << 1)     /* Power Management bit 1*/
#define ad7887_CTRL_PM0  		(1 << 0)     /* Power Management bit 0*/

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char AD7887_Init(void);

/* Receives 16 bits from SPI and transmits 2 control bytes. */
unsigned short AD7887_Read16Bits(unsigned char ctrl[2]);

#endif /* _AD7887_H_ */
