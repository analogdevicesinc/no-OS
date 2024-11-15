/***************************************************************************//**
 *   @file   ad7091.c
 *   @brief  Implementation of ADC AD7091 Driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/************************ Include Files ***************************************/
/******************************************************************************/
#include "ad7091.h"
#include "Communication.h"
#include "TIME.h"

/******************************************************************************/
/************************* Variables Definitions ******************************/
/******************************************************************************/
unsigned int this_baseAddress;

/******************************************************************************/
/*********************** Functions Definitions ********************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief
 *
 * @param - None
 *
 * @return - None
*******************************************************************************/
void AD7091_Init(unsigned int baseAddress)
{
	int registerValue = 0;

	this_baseAddress = baseAddress;
	registerValue = AD7091_GetRegister(AD7091_REG_RSTN);
	AD7091_SetRegister(AD7091_REG_RSTN, registerValue |
			   (~ADC_RSTN & 0x1));
}

/***************************************************************************//**
 * @brief Register write access
 *
 * @param -     address - address of the register
 *              data - data to be written into the register
 *
 * @return - None
*******************************************************************************/
void AD7091_SetRegister(unsigned int address, unsigned int data)
{
	SET_REGISTER((this_baseAddress + address), data);
}

/***************************************************************************//**
 * @brief Register read access
 *
 * @param - address - address of the register
 *
 * @return - None
*******************************************************************************/
unsigned int AD7091_GetRegister(unsigned int address)
{
	return GET_REGISTER(this_baseAddress + address);
}

/***************************************************************************//**
 * @brief Initiate data acquisition
 *
 * @param - size - the number of samples
 *
 * @return - 0 - if the transfer succeeded, otherwise -1
*******************************************************************************/
unsigned int AD7091_GetData(unsigned int size)
{
	int registerValue = 0;
	int status = 0;
	int timeOut = 0;

	// flush the cache to keep cache coherence
	FLUSH_CACHE();

	// Power up the core
	AD7091_SetRegister(AD7091_REG_RSTN, registerValue |
			   ADC_RSTN);
	// Clear DMA Status register
	registerValue = AD7091_GetRegister(AD7091_REG_DMA_STATUS);
	AD7091_SetRegister(AD7091_REG_DMA_STATUS, registerValue |
			   ADC_DMA_OVF |
			   ADC_DMA_UNF |
			   ADC_DMA_STATUS);
	// DMA stop
	registerValue = AD7091_GetRegister(AD7091_REG_DMA_CNTRL);
	AD7091_SetRegister(AD7091_REG_DMA_CNTRL, registerValue |
			   (~ADC_DMA_START & 0x1));

	// Program the number of bytes to be captured
	AD7091_SetRegister(AD7091_REG_DMA_COUNT, (unsigned int)(size * BUS_WIDTH));

	// start capturing data
	AD7091_SetRegister(AD7091_REG_DMA_CNTRL, registerValue |
			   ADC_DMA_START);

	timeOut = 0;
	// wait for end of the transaction OR ovf/unf assertion OR a 1s timeout
	do {
		TIME_DelayMs(10);
		status = AD7091_GetRegister(AD7091_REG_DMA_STATUS);
		timeOut++;
	} while(((status & 0x7) == 1) && (timeOut < 100));

	// Evaluate the transaction
	status = AD7091_GetRegister(AD7091_REG_DMA_STATUS);
	if((status & ADC_DMA_OVF) == ADC_DMA_OVF) {
		FLUSH_CACHE();
		return -1;
	} else {
		FLUSH_CACHE();
		return 0;
	}

}
