/**************************************************************************//**
*   @file   AD5111.c
*   @brief  Implementation of AD5111 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
