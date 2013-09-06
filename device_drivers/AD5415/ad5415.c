/***************************************************************************//**
*   @file   AD5415.c
*   @brief  Implementation of AD5415 Driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*******************************************************************************
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
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "AD5415.h"
#include "Communication.h"

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5415 Board.
 *
 * @param None.
 *
 * @return retValue - Result of the initialization.
 * 					  Example: 0 - if initialization was successful;
 *							  -1 - if initialization was unsuccessful.
******************************************************************************/
char AD5415_Init(void)
{
	char status = -1;

	/* Initialize SPI communication. */
	status = SPI_Init(0, 1000000, 0, 0);
	/* Set GPIO pins. */
	AD5415_LDAC_OUT;
	AD5415_LDAC_LOW;
	AD5415_CLR_OUT;
	AD5415_CLR_HIGH;
	/* Write to CTRL register. */
	AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_CTRL), 0x0);

	return status;
}

/**************************************************************************//**
 * @brief Write to input register and read from output register via SPI.
 *
 * @param	function - command control bits.
 * 			data	 - data to be written in input register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
unsigned short AD5415_SetInputShiftReg(unsigned short function,
							 	 	   unsigned short data)
{
	unsigned short inputShiftReg = 0;
	unsigned short readBack      = 0;
	unsigned char  spiData[2]    = {0, 0};

	inputShiftReg = function | (data & 0xFFF);
	spiData[0] = (inputShiftReg & 0xFF00) >> 8;
	spiData[1] = (inputShiftReg & 0x00FF) >> 0;
	SPI_Read(AD5415_SLAVE_ID, spiData, 2);
	readBack = ((unsigned short)spiData[0] << 8) |
			   (spiData[1] << 0);

	return readBack;
}

/**************************************************************************//**
 * @brief Loads and updates the selected DAC with a given value.
 *
 * @param	channel  - the chosen channel to write to.
 * 						Example: AD5415_CH_A = 0;
 * 								 AD5415_CH_B = 1.
 * 			dacValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5415_LoadUpdate(unsigned char channel, unsigned short dacValue)
{
    if (channel == AD5415_CH_A)
    {
    	AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_LOADUPDATEA), dacValue);
    }
    else
    {
    	if (channel == AD5415_CH_B)
    	{
    		AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_LOADUPDATEB), dacValue);
    	}
    }
}

/**************************************************************************//**
 * @brief Load selected DAC input register with a given value.
 *
 * @param	channel  - the chosen channel to write to.
 *   					Example: AD5415_CH_A = 0;
 * 								 AD5415_CH_B = 1.
 * 			dacValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5415_Load(unsigned char channel, unsigned short dacValue)
{
    if (channel == AD5415_CH_A)
    {
    	AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_LOADA), dacValue);
    }
    else
    {
    	if (channel == AD5415_CH_B)
    	{
    		AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_LOADB), dacValue);
    	}
    }
}

/**************************************************************************//**
 * @brief Read from the selected DAC register.
 *
 * @param	channel  - the chosen channel to read from.
 *   					Example: AD5415_CH_A = 0;
 * 								 AD5415_CH_B = 1.
 *
 * @return  dacValue - value read from the register.
******************************************************************************/
unsigned short AD5415_Readback(unsigned char channel)
{
    unsigned short dacValue = 0;

    if (channel == AD5415_CH_A)
    {
    	AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_READBACKA), 0x0);
    	dacValue = AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_NOOPBOTH),
    										 0x0) & 0xFFF;
    }
    else
    {
    	if (channel == AD5415_CH_B)
      	{
    	AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_READBACKB), 0x0);
    	dacValue = AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_NOOPBOTH),
    										 0x0) & 0xFFF;
      	}
    }

    return(dacValue);
}

/**************************************************************************//**
 * @brief Update both DAC outputs.
 *
 * @param  None.
 *
 * @return None.
******************************************************************************/
void AD5415_UpdateBoth(void)
{
	AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_UPDATEBOTH), 0x0);
}

/**************************************************************************//**
 * @brief Load both DAC input registers.
 *
 * @param dacValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5415_LoadBoth(short dacValue)
{
	AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_LOADBOTH), dacValue);
}

/**************************************************************************//**
 * @brief Clear both DAC outputs to zero scale or to to midscale.
 *
 * @param  type - target scale when making a clear command
 *  			  Example: ZERO_SCALE = zero scale.
 * 			 		       MIDSCALE   = midscale.
 *
 * @return None.
******************************************************************************/
void AD5415_Clear(char type)
{
	if (type == ZERO_SCALE)
	{
		AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_CLEARZERO), 0x0);
	}
	else
	{
		if (type == MIDSCALE)
		{
			AD5415_SetInputShiftReg(AD5415_CMD(AD5415_CMD_CLEARMID), 0x0);
		}
	}
}
