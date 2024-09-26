/***************************************************************************//**
*   @file   ad7887.h
*   @brief  Header file of ad7887 Driver for Microblaze processor.
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
