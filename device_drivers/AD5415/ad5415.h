/***************************************************************************//**
*   @file   AD5415.h
*   @brief  Header file of AD5415 Driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/* COMMAND Bytes */
#define AD5415_CMD_NOOPBOTH		    0
#define AD5415_CMD_LOADUPDATEA		1
#define AD5415_CMD_READBACKA      	2
#define AD5415_CMD_LOADA        	3
#define AD5415_CMD_LOADUPDATEB		4
#define AD5415_CMD_READBACKB      	5
#define AD5415_CMD_LOADB        	6
#define AD5415_CMD_UPDATEBOTH		7
#define AD5415_CMD_LOADBOTH			8
#define AD5415_CMD_DISDCHAIN		9
#define AD5415_CMD_CLEARZERO		11
#define AD5415_CMD_CLEARMID			12
#define AD5415_CMD_CTRL				13

/* AD5415 COMMAND mask */
#define AD5415_CMD(x)			((x & 0x0F) << 12)

/* AD5415 channels */
#define AD5415_CH_A				0
#define AD5415_CH_B				1

/* Clear target scales */
#define ZERO_SCALE				0
#define MIDSCALE				1

/* AD5415_CTRL_REG definition */
#define AD5415_CTRL_SDO1		(1 << 11)
#define AD5415_CTRL_SDO2		(1 << 10)
#define AD5415_CTRL_DSY			(1 << 9)
#define AD5415_CTRL_HCLR		(1 << 8)
#define AD5415_CTRL_SCLK		(1 << 7)

/* AD5415 GPIO */
#define AD5415_LDAC_OUT			GPIO1_PIN_OUT
#define AD5415_LDAC_LOW			GPIO1_LOW
#define AD5415_LDAC_HIGH		GPIO1_HIGH

#define AD5415_CLR_OUT			GPIO2_PIN_OUT
#define AD5415_CLR_LOW			GPIO2_LOW
#define AD5415_CLR_HIGH			GPIO2_HIGH

/* SPI slave device ID */
#define AD5415_SLAVE_ID    		1


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initialize SPI and Initial Values for AD5415 Board. */
char AD5415_Init(void);

/* Write to input register and read from output register via SPI. */
unsigned short AD5415_SetInputShiftReg(unsigned short function,
							 	 	   unsigned short data);

/* Load and updates the selected DAC with a given value. */
void AD5415_LoadUpdate(unsigned char channel, unsigned short dacValue);

/* Load selected DAC input register with a given value. */
void AD5415_Load(unsigned char channel, unsigned short dacValue);

/* Read from the selected DAC register. */
unsigned short AD5415_Readback(unsigned char channel);

/* Update both DAC outputs. */
void AD5415_UpdateBoth(void);

/* Load both DAC input registers. */
void AD5415_LoadBoth(short dacValue);

/* Clear both DAC outputs to zero scale or to to midscale. */
void AD5415_Clear(char type);
