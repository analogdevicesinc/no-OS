/***************************************************************************//**
*   @file   ad5662.h
*   @brief  Header file of ad5662 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
