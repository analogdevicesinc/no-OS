/***************************************************************************//**
 *   @file   ad7091.c
 *   @brief  Implementation of ADC AD7091 Driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*******************************************************************************/

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
