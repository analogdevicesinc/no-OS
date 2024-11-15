/***************************************************************************//**
*   @file   ad5750.h
*   @brief  Header file of ad5750 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _ad5750_H_
#define _ad5750_H_
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_types.h"
#include "xparameters.h"

/******************************************************************************/
/********************* Macros and Constants Definitions ***********************/
/******************************************************************************/
#define ad5750_SLAVE_ID         1

/******************************************************************************/
/* GPIOs                                                                      */
/******************************************************************************/
#define AD5750_CLR_OUT          GPIO0_PIN_OUT
#define AD5750_CLR_LOW          GPIO0_LOW
#define AD5750_CLR_HIGH         GPIO0_HIGH
#define AD5750_CLR_PIN          (1 << 0) // AD5750 Clear Pin.

#define AD5750_FAULT_IN         GPIO6_PIN_IN
#define AD5750_FAULT_PIN        (1 << 6) // AD5750 Fault Pin.
/******************************************************************************/
/* Shift Registers                                                            */
/******************************************************************************/
/* Input Shift Register Contents for a Write Operation � Control Register */
#define AD5750_CONTROL_ADDR(x)  (((x) & 0x7) << 13) /* Part address. */
#define AD5750_CONTROL_WRITE    (0 << 12)           /* Write bit. */
#define AD5750_CONTROL_RANGE(x) (((x) & 0xF) << 7)  /* Output range. */
#define AD5750_CONTROL_CLRSEL   (1 << 6)            /* Zero scale or midscale. */
#define AD5750_CONTROL_OUTEN    (1 << 5)            /* Output enable bit. */
#define AD5750_CONTROL_CLEAR    (1 << 4)            /* Software clear bit. */
#define AD5750_CONTROL_RSET     (1 << 3)            /* Select int current resistor.*/
#define AD5750_CONTROL_RESET    (1 << 2)            /* Resets the part to its power-on state. */

/* Input Shift Register Contents for a Read Operation � Status Register */
#define AD5750_STATUS_ADDR(x)    (((x) & 0x7) << 13)/* Part address. */
#define AD5750_STATUS_READ       (1 << 12)          /* Read bit. */
#define AD5750_STATUS_RANGE(x)   (((x) & 0xF) << 7) /* Output range. */
#define AD5750_STATUS_CLRSEL     (1 << 6)           /* Zero scale or midscale. */
#define AD5750_STATUS_OUTEN      (1 << 5)           /* Output enable bit. */
#define AD5750_STATUS_RSET       (1 << 4)           /* Select int current resistor.*/
#define AD5750_STATUS_PEC_ERR    (1 << 3)           /* Interface error. */
#define AD5750_STATUS_OVER_TEMP  (1 << 2)           /* Temperature error. */
#define AD5750_STATUS_IOUT_FAULT (1 << 1)           /* Open circuit on the IOUT pin. */
#define AD5750_STATUS_VOUT_FAULT (1 << 0)           /* Short circuit on the VOUT pin. */

/******************************************************************************/
/*************************** Functions Declarations ***************************/
/******************************************************************************/
/* Initializes the communication with the device. */
char ad5750_Init(void);

/* Writes 16 bits to device through SPI. */
char ad5750_Write16Bits(unsigned short data);

/* Reads 16 bits from the device through SPI. */
unsigned short ad5750_Read16Bits(unsigned short data);

#endif /* _ad5750_H_ */
