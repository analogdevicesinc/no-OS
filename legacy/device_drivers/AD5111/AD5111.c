/**************************************************************************//**
*   @file   AD5111.c
*   @brief  Implementation of AD5111 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD5111.h"
#include "Communication.h"
#include "TIME.h"

/******************************************************************************/
/*************************** Macros Definitions *******************************/
/******************************************************************************/
#define CS  1	// Chip select pin from Up/Down interface.
#define UnD 2   // Direction pin from Up/Down interface.
#define CLK 4	// Clock pin from Up/Down interface.

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Increases the resistor value with one step.
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void incValue(void)
{
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CLK);
	GPIO_SET_BITS(GPIO_BASEADDR, UnD);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CS);

	GPIO_SET_BITS(GPIO_BASEADDR, CLK);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CLK);
	GPIO_SET_BITS(GPIO_BASEADDR, CLK);

	GPIO_SET_BITS(GPIO_BASEADDR, CS);
	TIME_DelayUs(1);
}
/***************************************************************************//**
 * @brief Decreases the resistor value with one step.
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void decValue(void)
{
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CLK);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, UnD);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CS);

	GPIO_SET_BITS(GPIO_BASEADDR, CLK);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CLK);
	GPIO_SET_BITS(GPIO_BASEADDR, CLK);

	GPIO_SET_BITS(GPIO_BASEADDR, CS);
	TIME_DelayUs(1);
}

/***************************************************************************//**
 * @brief Device enters in shutdown mode.
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void shutDown(void)
{
	GPIO_CLEAR_BITS(GPIO_BASEADDR, UnD);
	GPIO_SET_BITS(GPIO_BASEADDR, CLK);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CS);
	GPIO_SET_BITS(GPIO_BASEADDR, UnD);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CLK);
}

/***************************************************************************//**
 * @brief Device memorizes the current RDAC value in EEPROM.
 *
 * @param none.
 *
 * @return none.
*******************************************************************************/
void saveMem(void)
{
	GPIO_CLEAR_BITS(GPIO_BASEADDR, UnD);
	GPIO_SET_BITS(GPIO_BASEADDR, CLK);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, CS);
	GPIO_SET_BITS(GPIO_BASEADDR, UnD);
	GPIO_CLEAR_BITS(GPIO_BASEADDR, UnD);
	TIME_DelayMs(20);
}

/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param none.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char AD5111_Init(void)
{
	unsigned char status = 0;

	// Set GPIO0, GPIO1 and GPIO2 as outputs
	GPIO_SET_PINS_OUT(GPIO_BASEADDR, 0x7);

	return status;
}
