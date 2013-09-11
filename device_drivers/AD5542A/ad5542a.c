/***************************************************************************//**
*   @file   ad5542a.c
*   @brief  Implementation of ad5542a Driver.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad5542a.h"

/***************************************************************************//**
 * @brief Initializes the communication peripheral and GPIO pins.
 *
 * @return Result of the initialization procedure.
 *            Example: -1 - SPI peripheral was not initialized.
 *                      0 - SPI peripheral is initialized.
*******************************************************************************/
char AD5542A_Init(void)
{
    unsigned char status = -1;

    status = SPI_Init(0, 1000000, 1, 0);
    AD5542A_LDAC_OUT;
    AD5542A_LDAC_LOW;
    AD5542A_CLR_OUT;
    AD5542A_CLR_HIGH;

    return status;
}

/**************************************************************************//**
 * @brief Writes to input register via SPI.
 *
 * @param	data - data to be written in input register.
 *
 * @return  None.
******************************************************************************/
void AD5542A_SetRegisterValue(unsigned short data)
{
	unsigned short inputShiftReg = 0;
	unsigned char  spiData[2]    = {0, 0};

	inputShiftReg = data & 0xFFFF;
	spiData[0] = (inputShiftReg & 0xFF00) >> 8;
	spiData[1] = (inputShiftReg & 0x00FF) >> 0;
	SPI_Write(AD5542A_SLAVE_ID, spiData, 2);
}

/***************************************************************************//**
 * @brief Sets the output voltage.
 *
 * @param outVoltage - The voltage value in volts
 * @param vRef       - The voltage reference used by the device in volts.
 *
 * @return Actual voltage that the device can output.
*******************************************************************************/
float AD5542A_SetVoltage(float outVoltage, float vRef)
{
    unsigned short  registerValue = 0;
    float 			actualVoltage = 0;
    float 			code          = 0;

    /* Get raw data from the user's desired voltage value. */
    code = ((outVoltage + vRef) * 32768) / vRef;
    /* Round to the nearest integer. */
    registerValue = (unsigned short)code;
    /* Check to value which will be written to register. */
    if (registerValue > 65535)
    {
    	registerValue = 65535;
    }

    /* Write to DAC register. */
    AD5542A_SetRegisterValue(registerValue);
    /* Calculate the output voltage value. */
    actualVoltage = (vRef * (float)registerValue / 32768) - vRef;

    return actualVoltage;
}
