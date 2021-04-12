/**************************************************************************//**
*   @file   ad527x.h
*   @brief  Header file of AD527x Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef _AD527x_H_
#define _AD527x_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/******************* Macros and Constants Definitions *************************/
/******************************************************************************/
/* Supported devices */
typedef enum {
	ID_AD5270,
	ID_AD5271,
	ID_AD5272,
	ID_AD5274,
} AD527x_type;

/* AD527x Device I2C Address */
#define AD527x_I2C_ADDR_0			0x2F // A1=1 and A0=1 (ADDR_Pin=GND)
#define AD527x_I2C_ADDR_1			0x2C // A1=0 and A0=0 (ADDR_Pin=VDD)
#define AD527x_I2C_ADDR_2			0x2E // A1=1 and A0=0 (ADDR_Pin=NC)

#define MAX_BITS_NUMBER  			10

/* Commands */
/* AD527x Commands */
#define AD527x_NOP					0x00
#define AD527x_WRITE_RDAC			0x01
#define AD527x_READ_RDAC			0x02
#define AD527x_STORE_RDAC_50TP		0x03
#define AD527x_RESET				0x04
#define AD527x_READ_CONTENT_50TP	0x05
#define AD527x_READ_ADDRESS_50TP	0x06
#define AD527x_WRITE_CONTROL		0x07
#define AD527x_READ_CONTROL			0x08
#define AD527x_SHUTDOWN				0x09

/* AD527x Control Register Bits */
#define AD527x_C3					(1 << 3)
#define AD527x_C2					(1 << 2)
#define AD527x_C1					(1 << 1)
#define AD527x_C0					(1 << 0)

/* AD527x GPIO */
/* RESET - GPIO1 */
#define AD527x_RESET_OUT        	GPIO1_PIN_OUT
#define AD527x_RESET_LOW        	GPIO1_LOW
#define AD527x_RESET_HIGH       	GPIO1_HIGH

#define AD527x_SLAVE_ID        		1

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initializes the communication with the device. */
char AD527x_Init(unsigned char devAddr, AD527x_type device);

/* Writes data to AD527x. */
void AD527x_SetRegister(unsigned char command, unsigned short data);

/* Reads data from AD527x. */
unsigned short AD527x_GetRegister(unsigned char command, unsigned char memReg50TP);

#endif /* AD527x_H_ */
