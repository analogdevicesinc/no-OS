/***************************************************************************//**
 *   @file   AD74xx.c
 *   @brief  Implementation of AD74xx Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD74xx.h"		// AD74xx definitions.

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/
char deviceBitsNumber = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes device.
 *
 * @param partNumber - The name of one of the supported devices.
 *                     Example: AD7466
 *                              AD7467
 *                              AD7468
 *                              AD7475
 *                              AD7476
 *                              AD7476A
 *                              AD7477
 *                              AD7477A
 *                              AD7478
 *                              AD7478A
 *                              AD7495
 *
 * @return status    - Result of the initialization procedure.
 *				Example: -1 - SPI peripheral was not initialized.
 *					  0 - SPI peripheral is initialized.
*******************************************************************************/
char AD74xx_Init(char partNumber)
{
    unsigned char status = -1;
    
    status = SPI_Init(0, 1000000, 1, 0);
    deviceBitsNumber = partNumber;
        
    return status;
}

/***************************************************************************//**
 * @brief Powers down the device. This function is supported only by: 
 *        AD7475(partial power down), AD7476, AD7477, AD7478, AD7476A, AD7477A,
 *        AD7478A, AD7479(partial power down).
 *
 * @return None.
*******************************************************************************/
void AD74xx_PowerDown(void)
{
    unsigned char dummyValue = 0;
    
    AD74XX_CS_LOW;
    SPI_Read(0, &dummyValue, 1);
    AD74XX_CS_HIGH;     // CS is brought "High" between 2nd falling edge of SCLK
                        // and 10th falling edge of SCLK(8th falling edge, here)
    SPI_Read(0, &dummyValue, 1);
}

/***************************************************************************//**
 * @brief Powers up the device by performing a dummy read. This function is 
 *        supported only by: AD7475(partial power down), AD7476, AD7477, AD7478,
 *        AD7476A, AD7477A, AD7478A, AD7479(partial power down).
 *
 * @return None.
*******************************************************************************/
void AD74xx_PowerUp(void)
{
    unsigned char dummyValue[2] = {0, 0};
    
    SPI_Read(AD74XX_SLAVE_ID, dummyValue, 2);
}

/***************************************************************************//**
 * @brief Reads the conversion value.
 *
 * @return convResult - conversion data.
*******************************************************************************/
unsigned short AD74xx_GetRegisterValue(void)
{
    unsigned char  dataWord[2] = {0, 0};
    unsigned short convResult  = 0;

    SPI_Read(AD74XX_SLAVE_ID, dataWord, 2);
    switch(deviceBitsNumber){
		case 8:
			convResult = (((unsigned short)(dataWord[0] & 0x1F)) << 8) +
                         (dataWord[1] & 0xE0);
            convResult = convResult >> 5;
			break;
		case 10:
			convResult = (((unsigned short)(dataWord[0] & 0x1F)) << 8) +
                         (dataWord[1] & 0xF8);
            convResult = convResult >> 3;
			break;
		case 12:
			convResult = (((unsigned short)(dataWord[0] & 0x1F)) << 8) +
                         (dataWord[1] & 0xFE);
            convResult = convResult >> 1;
			break;
		default: // 16 bits
			convResult = (((unsigned short)(dataWord[0] & 0xFF)) << 8) +
                         dataWord[1];
			break;		
	}
    
    return convResult;
}

/***************************************************************************//**
 * @brief Converts a raw sample to volts.
 *
 * @param rawValue - The data sample.
 * @param vRef     - The value of the voltage reference used by the device.
 *
 * @return voltage - The result of the conversion expressed as volts.
*******************************************************************************/
float AD74xx_ConvertToVolts(unsigned short rawValue, float vRef)
{
    float voltage = 0;
    
    voltage = (rawValue) * vRef / (1 << deviceBitsNumber);
        
    return voltage;
}
